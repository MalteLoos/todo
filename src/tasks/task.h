#ifndef TASK_H
#define TASK_H

#include <string>
#include <chrono>
#include <vector>

enum class Priority{LOW, MEDIUM, HIGH};
enum class Recurrence{NONE, DAILY, WEEKLY, MONTHLY, YEARLY};
enum class Category{WORK, EDUCATION, PERSONAL, HEALTH, FINANCE, OTHER};

class Task{
protected:
    std::string id; // => unique identifier for the task
    std::string title; // => task name 
    std::string description; // => descriptive details 
    std::chrono::system_clock::time_point deadline; // => due date for task completion
    Priority priority;; // => importance level    
    Recurrence recurrence; // => repetition patterns
    bool completed; // => task status (finished or not)
    Category category; // => task classification - thought "wouldn't make sense to have as subclasses"!
    std::vector<std::string> label; // =>organizational tags i.e #work #university #personal 
    std::string timePointToString(const std::chrono::system_clock::time_point& tp) const;

public:
    // CONSTRUCTORS 
    Task();
    Task(const std::string& title, 
        const std::chrono::system_clock::time_point& deadline,
        Priority priority,
        Category category,
        Recurrence recurrence);  

    // POLYMORPHISM 
    virtual ~Task() = default; // => destructer 

    // GETTERS
    std::string getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getDeadline() const;
    Priority getPriority() const;
    Category getCategory() const;
    Recurrence getRecurrence() const;
    bool isCompleted() const;
    std::vector<std::string> getLabel() const;

    // SETTERS
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void setPriority(Priority priority);
    void setRecurrence(Recurrence recurrence);
    void setCategory(Category category);
    void setLabel(const std::string& label);
    void setCompleted(bool completed);

    //UTILITIES
    bool isOverdue() const;

    virtual std::string toString() const;
    virtual std::string getType() const{
        return "Task";
    }

};

#endif //TASK_H