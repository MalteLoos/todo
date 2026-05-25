#ifndef TIMEDTASK_H
#define TIMEDTASK_H

#include "task.h"
class timedTask : public Task{
private:
    bool timerRunning; // => is the timer active  
    std::chrono::system_clock::time_point startTime; // => time when task should begin
    std::chrono::minutes duration; // => time required to carry the task
    std::chrono::minutes actualTime; // => time spent on task

public:

    // CONSTRUCTORS
    timedTask();
    timedTask(const std::string& title,
              const std::chrono::system_clock::time_point& deadline,
              Priority priority,
              Recurrence recurrence,
              const std::chrono::system_clock::time_point& startTime,
              const std::chrono::minutes& duration);

    // GETTERS
    bool isTimerRunning() const;
    std::chrono::system_clock::time_point getStartTime() const;
    std::chrono::minutes getDuration() const;
    std::chrono::system_clock::time_point getEndTime() const;
    std::chrono::minutes getActualTime() const;

    //SETTERS
    void setTimerRunning(bool running);
    void setStartTime(const std::chrono::system_clock::time_point& startTime);
    void setDuration(std::chrono::minutes duration);
    void setActualTime(std::chrono::minutes actualTime);
    

    // UTILITIES
    void startTimer();
    void stopTimer();
    std::string toString() const override;
};

#endif //TIMEDTASK_H