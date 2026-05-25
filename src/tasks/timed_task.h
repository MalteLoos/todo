#ifndef TIMEDTASK_H
#define TIMEDTASK_H

#include "task.h"
class timedTask : public Task{
private:
    std::chrono::system_clock::time_point startTime; // => time when task should begin
    std::chrono::system_clock::time_point duration; // => time required to carry the task
public:
    // CONSTRUCTORS
    timedTask();
    timedTask(const std::string& title,
              const std::chrono::system_clock::time_point& deadline,
              Priority priority,
              Recurrence recurrence,
              const std::chrono::system_clock::time_point& startTime,
              const std::chrono::system_clock::time_point& duration);

    // GETTERS
    std::chrono::system_clock::time_point getStartTime() const;
    std::chrono::minutes getDuration() const;
    std::chrono::system_clock::time_point getEndTime() const;

    //SETTERS
    void setStartTime(const std::chrono::system_clock::time_point& startTime);
    void setDuration(std::chrono::minutes duration);

    // UTILITIES
    std::string toString() const override;
};

#endif //TIMEDTASK_H