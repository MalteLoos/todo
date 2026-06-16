#include "client.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

// Claude, code was edited afterwards: implement the client with tcp keepalive:
TaskClient::TaskClient(const std::string& host, int port, const std::string& userId) {
    // create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) throw std::runtime_error("Failed to create socket");

    // connect to address
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
        throw std::runtime_error("Invalid address");

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Connection failed");

    // keepalive
    int yes = 1, idle = 10, interval = 5, count = 3;
    setsockopt(fd, SOL_SOCKET,  SO_KEEPALIVE,  &yes,      sizeof(yes));
#ifdef TCP_KEEPIDLE
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,  &idle,     sizeof(idle));
#elif defined(TCP_KEEPALIVE)
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &idle,     sizeof(idle));
#endif
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,   &count,    sizeof(count));

    // authenticate
    std::vector<uint8_t> auth(userId.begin(), userId.end());
    if (!sendMessage(auth)) throw std::runtime_error("Authentication failed");

    // start notification loop
    running = true;
    notifyThread = std::thread(&TaskClient::notifyLoop, this);
}

TaskClient::~TaskClient() {
    running = false;
    if (fd >= 0) close(fd);
    if (notifyThread.joinable()) notifyThread.join();
}

void TaskClient::setNotifyCallback(std::function<void(Type, std::vector<std::unique_ptr<Task>>)> cb) {
    onNotify = std::move(cb);
}

bool TaskClient::sendMessage(const std::vector<uint8_t>& data) {
    std::thread([this, data]() {
        uint32_t len = htonl(static_cast<uint32_t>(data.size()));
        send(fd, &len, 4, MSG_NOSIGNAL);
        send(fd, data.data(), data.size(), MSG_NOSIGNAL);
    }).detach();
    return true;
}

std::optional<Msg> TaskClient::receiveMessage(Type expected) {
    std::unique_lock lock(queueMtx);
    queueCv.wait(lock, [&] {
        // scan queue for a matching type
        std::queue<Msg> tmp;
        bool found = false;
        while (!msgQueue.empty()) {
            if (!found && msgQueue.front().type == expected) {
                found = true;  // will extract below
            }
            tmp.push(std::move(msgQueue.front()));
            msgQueue.pop();
        }
        msgQueue = std::move(tmp);
        return found || !running;
    });

    // extract first matching message
    std::queue<Msg> tmp;
    std::optional<Msg> result;
    while (!msgQueue.empty()) {
        if (!result && msgQueue.front().type == expected)
            result = std::move(msgQueue.front());
        else
            tmp.push(std::move(msgQueue.front()));
        msgQueue.pop();
    }
    msgQueue = std::move(tmp);
    return result;
}

void TaskClient::notifyLoop() {
    while (running) {
        uint8_t lenBuf[4];
        if (recv(fd, lenBuf, 4, MSG_WAITALL) <= 0) break;
        uint32_t len = ntohl(*reinterpret_cast<uint32_t*>(lenBuf));
        if (len == 0 || len > 10 * 1024 * 1024) break;
        std::vector<uint8_t> buf(len);
        if (recv(fd, buf.data(), len, MSG_WAITALL) <= 0) break;

        Msg msg = Msg::deserialize(buf);
        if ((msg.type == Type::NOTIFY || msg.type == Type::ADD ||
             msg.type == Type::UPDATE || msg.type == Type::DELETE) && onNotify) {
            onNotify(msg.type, std::move(msg.tasks));
        } else {
            std::unique_lock lock(queueMtx);
            msgQueue.push(std::move(msg));
            queueCv.notify_all();
        }
    }
    running = false;
    queueCv.notify_all();
}

std::vector<std::unique_ptr<Task>> TaskClient::getTasks() {
    sendMessage(Msg{Type::GETALL, {}}.serialize());
    auto resp = receiveMessage(Type::GETALL);
    if (!resp) return {};
    return std::move(resp->tasks);
}

void TaskClient::addTask(const Task& task) {
    std::vector<std::unique_ptr<Task>> t;
    t.push_back(std::make_unique<Task>(task));
    sendMessage(Msg{Type::ADD, std::move(t)}.serialize());
}

void TaskClient::updateTask(const Task& task) {
    std::vector<std::unique_ptr<Task>> t;
    t.push_back(std::make_unique<Task>(task));
    sendMessage(Msg{Type::UPDATE, std::move(t)}.serialize());
}

void TaskClient::deleteTask(const std::string& taskId) {
    std::vector<std::unique_ptr<Task>> t;
    auto task = std::make_unique<Task>();
    task->setId(taskId);
    t.push_back(std::move(task));
    sendMessage(Msg{Type::DELETE, std::move(t)}.serialize());
}
