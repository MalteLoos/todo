#include "tasks/recurring_task.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

 
// CONSTRUCTORS
recurringTask::recurringTask()
    : Task(), // => superclass constructor
    nextOccurrence(std::chrono::system_clock::now())
{} 

recurringTask::recurringTask(const std::string& title,
                             const std::chrono::system_clock::time_point& deadline,
                             Priority priority,
                             Recurrence rec)
    : Task(title, deadline, priority, Category::OTHER, rec),
      nextOccurrence(deadline){}

// GETTERS 
Recurrence recurringTask::getRecurrence() const{
    return recurrence;
}

std::chrono::system_clock::time_point recurringTask::getNextOccurrence() const{
    return nextOccurrence;
}
// SETTERS
void recurringTask::setRecurrence(Recurrence rec){
    recurrence = rec;
}

void recurringTask::setNextOccurrence(const std::chrono::system_clock::time_point& next){
    nextOccurrence = next;
}

// UTILITIES
void recurringTask::markCompleted() {
    completed = true;
    if (recurrence != Recurrence::NONE) {
        deadline = calculateNextOccurrence(deadline); // => to schedule next occurrence  
        completed = false;  // => Reset for next cycle
        nextOccurrence = deadline;
    }
}

std::chrono::system_clock::time_point recurringTask::calculateNextOccurrence(
    const std::chrono::system_clock::time_point& getCurrentDeadline) const {
    
    using namespace std::chrono;
    
    auto nextTime = getCurrentDeadline;
    
    switch (recurrence) {
        case Recurrence::DAILY:
            nextTime += hours(24);
            break;
        case Recurrence::WEEKLY:
            nextTime += hours(24 * 7);
            break;
        case Recurrence::MONTHLY:
            nextTime += hours(24 * 30);  
            break;
        case Recurrence::YEARLY:
            nextTime += hours(24 * 365);
            break;
        case Recurrence::NONE:
        default:
            break;
    }
    return nextTime;
}

std::unique_ptr<recurringTask> recurringTask::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string type, id, title, description, deadlineStr, priorityStr, categoryStr, recurrenceStr, completedStr, nextOccurrenceStr;

    if (!std::getline(ss, type, '|') || type != "RECURRING" ||
        !std::getline(ss, id, '|') ||
        !std::getline(ss, title, '|') ||
        !std::getline(ss, description, '|') ||
        !std::getline(ss, deadlineStr, '|') ||
        !std::getline(ss, priorityStr, '|') ||
        !std::getline(ss, categoryStr, '|') ||
        !std::getline(ss, recurrenceStr, '|') ||
        !std::getline(ss, completedStr, '|') ||
        !std::getline(ss, nextOccurrenceStr))
        return nullptr;

    auto deadline = std::chrono::system_clock::from_time_t(std::stoll(deadlineStr));
    auto nextOccurrence = std::chrono::system_clock::from_time_t(std::stoll(nextOccurrenceStr));
    auto task = std::make_unique<recurringTask>(
        title, deadline,
        static_cast<Priority>(std::stoi(priorityStr)),
        static_cast<Recurrence>(std::stoi(recurrenceStr))
    );
    task->setId(id);
    task->setDescription(description);
    task->setCategory(static_cast<Category>(std::stoi(categoryStr)));
    task->setCompleted(completedStr == "1");
    task->setNextOccurrence(nextOccurrence);
    return task;
}

std::string recurringTask::serialize() const {
    std::stringstream ss;
    ss << "RECURRING|" << serializeBase() << "|"
       << std::chrono::system_clock::to_time_t(nextOccurrence);
    return ss.str();
}

std::string recurringTask::toString() const {
    std::stringstream ss;
    ss << Task::toString();  
    ss << "Type: " << getType() << "\n";
    ss << "Next Occurrence: " << std::chrono::system_clock::to_time_t(nextOccurrence) << "\n";
    return ss.str();
}

void recurringTask::setCompleted(bool completed) {
    if (completed) {
        markCompleted();
    } else {
        Task::setCompleted(completed);
    }
}

std::unique_ptr<Task> recurringTask::clone() const {
    return std::make_unique<recurringTask>(*this);
}

