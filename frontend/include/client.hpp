#pragma once
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <thread>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "task.h"
#include "msg.hpp"

class TaskClient {
    public:
        TaskClient(const std::string& host, int port, const std::string& userId);
        ~TaskClient();

        std::vector<std::unique_ptr<Task>> getTasks();
        std::unique_ptr<Task> getTaskInfo(const std::string& taskId);
        void addTask(const Task& task);
        void updateTask(const Task& task);
        void deleteTask(const std::string& taskId);

        void setNotifyCallback(std::function<void(Type, std::vector<std::unique_ptr<Task>>)> cb);

    private:
        int fd = -1;
        std::thread notifyThread;
        std::atomic<bool> running{false};
        std::function<void(Type, std::vector<std::unique_ptr<Task>>)> onNotify;

        std::mutex queueMtx;
        std::condition_variable queueCv;
        std::queue<Msg> msgQueue;

        bool sendMessage(const std::vector<uint8_t>& data);
        std::optional<Msg> receiveMessage(Type expected);
        void notifyLoop();
};
