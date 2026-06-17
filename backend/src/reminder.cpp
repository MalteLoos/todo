#include "reminder.hpp"

static std::chrono::system_clock::duration recurrenceInterval(Recurrence r) {
    using namespace std::chrono;
    switch (r) {
        case Recurrence::DAILY:   return hours(24);
        case Recurrence::WEEKLY:  return hours(24 * 7);
        case Recurrence::MONTHLY: return hours(24 * 30);
        case Recurrence::YEARLY:  return hours(24 * 365);
        default:                  return hours(0);
    }
}

ReminderScheduler::ReminderScheduler(NotifyFn fn)
    : notify(std::move(fn)),
      worker(&ReminderScheduler::loop, this) {}

ReminderScheduler::~ReminderScheduler() {
    {
        std::unique_lock lock(mtx);
        running = false;
    }
    cv.notify_all();
    if (worker.joinable()) worker.join();
}

void ReminderScheduler::update(const std::vector<std::unique_ptr<Task>>& tasks) {
    auto now = std::chrono::system_clock::now();

    std::unique_lock lock(mtx);

    // rebuild queue: only schedule incomplete tasks with future deadlines
    decltype(queue) fresh;
    for (const auto& t : tasks) {
        if (!t || t->isCompleted()) continue;
        auto deadline = t->getDeadline();
        if (deadline > now)
            fresh.push({deadline, std::make_shared<Task>(*t)});
    }
    queue = std::move(fresh);
    cv.notify_all();
}

void ReminderScheduler::loop() {
    while (true) {
        std::unique_lock lock(mtx);

        // sleep until next deadline or until woken by update/stop
        if (queue.empty()) {
            cv.wait(lock, [&] { return !queue.empty() || !running; });
        } else {
            auto next = queue.top().fireAt;
            cv.wait_until(lock, next, [&] {
                // wake early if queue was updated or we're stopping
                return queue.empty() || queue.top().fireAt != next || !running;
            });
        }

        if (!running) break;
        if (queue.empty()) continue;

        auto now = std::chrono::system_clock::now();
        // fire all entries whose deadline has passed
        while (!queue.empty() && queue.top().fireAt <= now) {
            auto entry = queue.top();
            queue.pop();
            lock.unlock();
            notify(*entry.task);
            lock.lock();
            
            // Claude: fix timer for reoccuring tasks
            auto rec = entry.task->getRecurrence();
            if (rec != Recurrence::NONE)
                queue.push({entry.fireAt + recurrenceInterval(rec), entry.task});
        }
    }
}
