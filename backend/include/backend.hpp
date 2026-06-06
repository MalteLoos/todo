#pragma once
#include <vector>
#include <thread>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <mutex>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>
#include "taskStorage.h"
#include "reminder.hpp"

struct User {
    std::string id;
    std::vector<int> connections;
    std::shared_ptr<taskStorage> storage;
    std::unique_ptr<ReminderScheduler> reminder;

    User(const std::string& id) : id(id), storage(std::make_shared<taskStorage>(id + "/data.json")) {}
};

class Backend {
    public:
        void acceptLoop(int serverFd);

    private:
        std::mutex usersMtx;
        std::unordered_map<std::string, std::shared_ptr<User>> users;

        void handleConnection(int fd);
        void setKeepalive(int fd);
        std::string receiveAuth(int fd);
        std::optional<std::vector<uint8_t>> receiveMessage(int fd);

        bool sendMessage(int fd, const std::vector<uint8_t>& data);
        void notifyUser(int sender, const std::string& userId, const std::vector<uint8_t>& data);

        void processMessage(std::string userId, int fd, std::vector<uint8_t>& msg);
        void ensureReminder(const std::string& userId);
        void refreshReminder(const std::string& userId);
};