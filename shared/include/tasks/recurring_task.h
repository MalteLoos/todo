#ifndef RECURRING_TASK
#define RECURRING_TASK

#include "task.h"

class recurringTask : public Task { // => recurringTask is subclass to Task superclass
private:
    std::chrono::system_clock::time_point nextOccurrence; // => next scheduling 

public:
    // CONSTRUCTOR
    recurringTask();
    recurringTask(const std::string& title,
                const std::chrono::system_clock::time_point& deadline,
                Priority priority,
                Recurrence recurrence);

    // GETTERS 
    Recurrence getRecurrence() const;
    std::chrono::system_clock::time_point getNextOccurrence() const;


    //SETTERS
    void setRecurrence(Recurrence recurrence);
    void setNextOccurrence(const std::chrono::system_clock::time_point& nextOccurrence);


    // UTILITIES
    void markCompleted();
    std::string serialize() const override;
    static std::unique_ptr<recurringTask> deserialize(const std::string& line);
    std::unique_ptr<Task> clone() const override;
    std::string toString() const override;
    std::string getType() const override {
        return "Recurring Task";
    }
    void setCompleted(bool completed) override;

private:
    std::chrono::system_clock::time_point calculateNextOccurrence(const std::chrono::system_clock::time_point& getCurrentDeadline) const;
};

#endif //RECURRING_TASK