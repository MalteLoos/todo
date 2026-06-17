#pragma once
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>
#include "tasks/task.h"

class ReminderScheduler {
public:
    // callback receives the task that is due
    using NotifyFn = std::function<void(const Task&)>;

    ReminderScheduler(NotifyFn fn);
    ~ReminderScheduler();

    // rebuild the queue from current task list (call on add/update/delete/getall)
    void update(const std::vector<std::unique_ptr<Task>>& tasks);

private:
    struct Entry {
        std::chrono::system_clock::time_point fireAt;
        std::shared_ptr<Task> task;

        bool operator>(const Entry& o) const { return fireAt > o.fireAt; }
    };

    NotifyFn notify;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = true;

    // min-heap: earliest deadline on top
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> queue;

    void loop();
};
