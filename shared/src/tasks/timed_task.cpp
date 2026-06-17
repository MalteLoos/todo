#include <sstream>
#include <iomanip>

#include "tasks/timed_task.h"

// CONSTRUCTORS

timedTask::timedTask() 
    : Task(), // Invokes parent default constructor
      timerRunning(false),
      startTime(std::chrono::system_clock::now()),
      duration(std::chrono::minutes(0)),
      actualTime(std::chrono::minutes(0)) {}

timedTask::timedTask(const std::string& title,
                     const std::chrono::system_clock::time_point& deadline,
                     Priority priority,
                     Recurrence recurrence,
                     const std::chrono::system_clock::time_point& startTime,
                     const std::chrono::minutes& duration)
    : Task(title, deadline, priority, Category::WORK, recurrence),
      timerRunning(false),
      startTime(startTime),
      duration(duration),
      actualTime(std::chrono::minutes(0)) // => Starts at zero tracking minutes
      {}

    // GETTERS

bool timedTask::isTimerRunning() const {
    return timerRunning;
}

std::chrono::system_clock::time_point timedTask::getStartTime() const {
    return startTime;
}

std::chrono::minutes timedTask::getDuration() const {
    return duration;
}

std::chrono::system_clock::time_point timedTask::getEndTime() const {
    return startTime + std::chrono::duration_cast<std::chrono::system_clock::duration>(duration);
}

std::chrono::minutes timedTask::getActualTime() const {
    return actualTime;
}

//SETTERS

void timedTask::setTimerRunning(bool running) {
    timerRunning = running;
}

void timedTask::setStartTime(const std::chrono::system_clock::time_point& st) {
    startTime = st;
}

void timedTask::setDuration(std::chrono::minutes d) {
    duration = d;
}

void timedTask::setActualTime(std::chrono::minutes at) {
    actualTime = at;
}

// UTILITIES

void timedTask::startTimer() {
    timerRunning = true;
    startTime = std::chrono::system_clock::now();
}

void timedTask::stopTimer() {
    timerRunning = false;
}

std::unique_ptr<timedTask> timedTask::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string type, id, title, description, deadlineStr, priorityStr, categoryStr, recurrenceStr, completedStr;
    std::string startTimeStr, durationStr, actualTimeStr, timerRunningStr;

    if (!std::getline(ss, type, '|') || type != "TIMED" ||
        !std::getline(ss, id, '|') ||
        !std::getline(ss, title, '|') ||
        !std::getline(ss, description, '|') ||
        !std::getline(ss, deadlineStr, '|') ||
        !std::getline(ss, priorityStr, '|') ||
        !std::getline(ss, categoryStr, '|') ||
        !std::getline(ss, recurrenceStr, '|') ||
        !std::getline(ss, completedStr, '|') ||
        !std::getline(ss, startTimeStr, '|') ||
        !std::getline(ss, durationStr, '|') ||
        !std::getline(ss, actualTimeStr, '|') ||
        !std::getline(ss, timerRunningStr))
        return nullptr;

    auto deadline = std::chrono::system_clock::from_time_t(std::stoll(deadlineStr));
    auto startTime = std::chrono::system_clock::from_time_t(std::stoll(startTimeStr));
    auto task = std::make_unique<timedTask>(
        title, deadline,
        static_cast<Priority>(std::stoi(priorityStr)),
        static_cast<Recurrence>(std::stoi(recurrenceStr)),
        startTime,
        std::chrono::minutes(std::stoll(durationStr))
    );
    task->setId(id);
    task->setDescription(description);
    task->setCategory(static_cast<Category>(std::stoi(categoryStr)));
    task->setCompleted(completedStr == "1");
    task->setActualTime(std::chrono::minutes(std::stoll(actualTimeStr)));
    task->setTimerRunning(timerRunningStr == "1");
    return task;
}

std::string timedTask::serialize() const {
    std::stringstream ss;
    ss << "TIMED|" << serializeBase() << "|"
       << std::chrono::system_clock::to_time_t(startTime) << "|"
       << duration.count() << "|"
       << actualTime.count() << "|"
       << (timerRunning ? "1" : "0");
    return ss.str();
}

std::unique_ptr<Task> timedTask::clone() const {
    return std::make_unique<timedTask>(*this);
}

std::string timedTask::toString() const {
    std::stringstream ss;
    ss << Task::toString() << "\n";
    ss << "Start Time: " << startTime.time_since_epoch().count() << "\n";
    ss << "Duration: " << duration.count() << " minutes\n";
    ss << "Actual Time: " << actualTime.count() << " minutes\n";
    ss << "Timer Running: " << (timerRunning ? "Yes" : "No") << "\n";
    return ss.str();
}