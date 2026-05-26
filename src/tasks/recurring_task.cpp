#include "recurring_task.h"
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

std::string recurringTask::toString() const {
    std::stringstream ss;
    ss << Task::toString();  
    ss << "Type: " << getType() << "\n";
    ss << "Next Occurrence: " << std::chrono::system_clock::to_time_t(nextOccurrence) << "\n";
    return ss.str();
}

