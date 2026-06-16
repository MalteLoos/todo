#include "backend.hpp"
#include <algorithm>
#include "msg.hpp"
#include "reminder.hpp"

// Claude, code was edited afterwards: how to write a server that handles multiple users with multiple clients each:

// accept incomming connections
void Backend::acceptLoop(int serverFd) {
    while (true) {
        int clientFd = accept(serverFd, nullptr, nullptr);
        if (clientFd < 0) continue;
        setKeepalive(clientFd); // keep connection open to send updates
        std::thread(&Backend::handleConnection, this, clientFd).detach();
    }
}

// recive msgs from client until connection dies
void Backend::handleConnection(int fd) {
    // first message must be authentication
    std::string userId = receiveAuth(fd);
    if (userId.empty()) { close(fd); return; }

    // add new connection to user and start reminder if needed
    {
        std::unique_lock lock(usersMtx);
        if (!users.contains(userId))
            users[userId] = std::make_shared<User>(userId);
        users[userId]->connections.push_back(fd);
        ensureReminder(userId);
        refreshReminder(userId);
    }

    // receive loop
    while (true) {
        auto msg = receiveMessage(fd);
        if (!msg) break; // connection lost
        processMessage(userId, fd, *msg);
    }

    // cleanup, remove connection from user
    std::unique_lock lock(usersMtx);
    auto& conns = users[userId]->connections;
    conns.erase(
        std::remove_if(conns.begin(), conns.end(),
            [fd](const auto& c) { return c == fd; }),
        conns.end()
    );
    close(fd);
}

// recive msgs
std::optional<std::vector<uint8_t>> Backend::receiveMessage(int fd) {
    uint8_t lenBuf[4];
    if (recv(fd, lenBuf, 4, MSG_WAITALL) <= 0) return std::nullopt;
    
    uint32_t len = ntohl(*reinterpret_cast<uint32_t*>(lenBuf));
    if (len == 0 || len > 10 * 1024 * 1024) return std::nullopt; // sanity check
    
    std::vector<uint8_t> buf(len);
    if (recv(fd, buf.data(), len, MSG_WAITALL) <= 0) return std::nullopt;
    
    return buf;
}

// send msg
bool Backend::sendMessage(int fd, const std::vector<uint8_t>& data) {
    uint32_t len = htonl(static_cast<uint32_t>(data.size()));
    if (send(fd, &len, 4, MSG_NOSIGNAL) != 4) return false;
    if (send(fd, data.data(), data.size(), MSG_NOSIGNAL) != static_cast<ssize_t>(data.size())) return false;
    return true;
}

// get user id from first msg
std::string Backend::receiveAuth(int fd) {
    auto msg = receiveMessage(fd);
    if (!msg) return "";
    return std::string(msg->begin(), msg->end());
}

// notify users on updates, send reminders
void Backend::notifyUser(int sender, const std::string& userId, const std::vector<uint8_t>& data) {
    std::unique_lock lock(usersMtx);
    auto it = users.find(userId);
    if (it == users.end()) return;
    for (auto& conn : it->second->connections) {
        if (sender == conn) continue;
        sendMessage(conn, data);
    }
}

// Claude: how to make this server keepalive:
void Backend::setKeepalive(int fd) {
    int yes = 1;
    int idle = 10;
    int interval = 5;
    int count = 3;

    setsockopt(fd, SOL_SOCKET,  SO_KEEPALIVE,  &yes,      sizeof(yes));
#ifdef TCP_KEEPIDLE
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,  &idle,     sizeof(idle));
#elif defined(TCP_KEEPALIVE)
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &idle,     sizeof(idle));
#endif
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,   &count,    sizeof(count));
}

// start reminder scheduler for user if not already running
void Backend::ensureReminder(const std::string& userId) {
    auto& user = users[userId];
    if (user->reminder) return;
    user->reminder = std::make_unique<ReminderScheduler>([this, userId](const Task& task) {
        // send NOTIFY with the due task to all connections of this user
        std::vector<std::unique_ptr<Task>> tasks;
        tasks.push_back(std::make_unique<Task>(task));
        auto data = Msg{Type::NOTIFY, std::move(tasks)}.serialize();
        std::unique_lock lock(usersMtx);
        auto it = users.find(userId);
        if (it == users.end()) return;
        for (int conn : it->second->connections)
            sendMessage(conn, data);
    });
}

// reload tasks from storage into the reminder queue (call after any mutation)
void Backend::refreshReminder(const std::string& userId) {
    auto& user = users[userId];
    if (!user->reminder) return;
    std::vector<std::unique_ptr<Task>> tasks;
    user->storage->loadTasks(tasks);
    user->reminder->update(tasks);
}

// processincoming msgs
void Backend::processMessage(std::string userId, int fd, std::vector<uint8_t>& msg) {
    // read msg
    Msg m;
    try {
        m = Msg::deserialize(msg);
    } catch (std::runtime_error error) {
        // msg could not be read
        std::vector<std::unique_ptr<Task>> errTasks;
        errTasks.push_back(std::make_unique<Task>("malformed msg recived"));
        sendMessage(fd, Msg{Type::ERROR, std::move(errTasks)}.serialize());
    }

    // process msg
    std::unique_lock lock(usersMtx);
    auto storage = users[userId]->storage;
    switch(m.type) {
        case Type::ADD:
            // add task: assumes only one task was sent
            if (!m.tasks.empty()) {
                auto notify = Msg{Type::ADD, {}};
                notify.tasks.push_back(std::make_unique<Task>(*m.tasks.front()));
                storage->addTask(std::move(m.tasks.front()));
                refreshReminder(userId);
                lock.unlock();
                notifyUser(fd, userId, notify.serialize());
            }
            break;
        case Type::GET: {
                // get one task
                std::unique_ptr<Task> task;
                storage->loadTask(task);
                std::vector<std::unique_ptr<Task>> tasks;
                if (task) tasks.push_back(std::move(task));
                sendMessage(fd, Msg{Type::GET, std::move(tasks)}.serialize());
            }
            break;
        case Type::GETALL: {
                // all tasks
                std::vector<std::unique_ptr<Task>> tasks;
                storage->loadTasks(tasks);
                refreshReminder(userId);
                sendMessage(fd, Msg{Type::GETALL, std::move(tasks)}.serialize());
            }
            break;
        case Type::UPDATE:
            // update the task and notify others
            if (!m.tasks.empty()) {
                auto notify = Msg{Type::UPDATE, {}};
                notify.tasks.push_back(std::make_unique<Task>(*m.tasks.front()));
                storage->updateTask(m.tasks.front());
                refreshReminder(userId);
                lock.unlock();
                notifyUser(fd, userId, notify.serialize());
            }
            break;
        case Type::DELETE:
            // delete the task and notify others
            if (!m.tasks.empty()) {
                auto notify = Msg{Type::DELETE, {}};
                notify.tasks.push_back(std::make_unique<Task>(*m.tasks.front()));
                storage->deleteTask(m.tasks.front());
                refreshReminder(userId);
                lock.unlock();
                notifyUser(fd, userId, notify.serialize());
            }
            break;
    }
}
