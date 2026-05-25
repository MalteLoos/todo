#ifndef RECURRING_TASK
#define RECURRING_TASK

#include "task.h"

class recurringTask : public Task { // => recurringTask is subclass to Task superclass
    private:
    Recurrence recurrence; // => pattern of repetition
    std::chrono::system_clock::time_point nextOcurrence; // => next scheduling 

    public:
    // CONSTRUCTOR
    recurringTask();
    recurringTask(const std::string& title,
                const std::chrono::system_clock::time_point& deadline,
                Priority prioriity,
                Recurrence recurrence);

    // GETTERS 
    Recurrence getRecurrence() const;
    std::chrono::system_clock::time_point getNextOccurrence() const;


    //SETTERS
    void setRecurrence(Recurrence recurrence);


    // UTILITIES
    void markCompleted(); 
    std::string toString() const override;
    std::string getType() const override;
};

private: 
    std::chrono::system_clock::time_point calculateNextOcurrence(const std::chrono::system_clock::time_point& getCurrentDeadline() const);
};


#endif //RECURRING_TASK