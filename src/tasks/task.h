#ifndef TASK_H
#define TASK_H

#include <string>
#include<chrono>

enum class Priority{LOW, MEDIUM, HIGH};
enum class Recurrence{NONE, DAILY, WEEKLY, MONTHLY, YEARLY};

class Task{
protected:
    std::string id; // => unique identifier for the task
    std::string title; // => task name 
    std::string description; // => descriptive details 
    std::chrono::system_clock::time_point deadline; // => due date for task completion
    Priority priority;; // => importance level    
    Recurrence recurrence; // => repetition patterns
    bool completed; // => task status (finished or not)
    std::vector<std::string> label; // =>organizational tags i.e #work #university #personal 

public:
    // CONSTRUCTORS 
    Task();
    Task(const std::string& title, 
        const std::chrono::system_clock::time_point& deadline,
        Priority priority,
        Recurrence recurrence);  

    // GETTERS
    std::string getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getDeadline() const;
    Priority getPriority() const;
    Recurrence getRecurrence() const;
    bool isCompleted() const;
    std::vector<std::string> getLabel() const;

    // SETTERS
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void setPriority(Priority priority);
    void setRecurrence(Recurrence recurrence);
    void setLabel(const std::string& label);
    void setCompleted(bool completed);

    //UTILITIES
    bool isOverdue() const;
    std::string toString() const;

};

#endif //TASK_H