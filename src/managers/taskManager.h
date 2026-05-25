// Planned task operations: add - delete - update - finish - search - filter - sort 

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <map>
#include <string>
#include <vector>

#include "../tasks/task.h" // => taskManager relies on Task definition

class TaskManager{
private:
    std::vector<std::unique_ptr<Task>> tasks;

public:
    // CONSTRUCTOR
    TaskManager() = default;
    
    void addTask(const Task& task);
    bool deleteTask(const std::string& id);
    bool updateTask(const std::string& id, const Task& updatedTask);
    bool finishTask(const std::string& id);

    // ALGORITHMS
    Task* searchById(const std::string& id);

    std::vector<std::unique_ptr<Task>> searchByTitle(const std::string& keyword) const; // => searching tasks whose title contains a specific >> keyword<<

    std::vector<Task*> filterByPriority(Priority priority) const; // => filtering tasks based on priority leve
    std::vector<Task*> filterByStatus(bool completed) const; // => filtering tasks aka. get the completed or incomplted ones
    std::vector<Task*> getOverdueTasks() const; // => retrieve tasks past deadline 

    std::vector<Task*> getTasksSortedByDeadline() const; // => sorting tasks by their deadlines
    std::vector<Task*> getTasksSortedByPriority() const; // => sort: highest to lowest 

    std::vector<Task*> getRecurringTasks() const; // => retrieve all repeated tasks
    std::vector<Task*> getTimedTasks() const; // => retrieved all tasks w/ timer
    
    //UTILITIES
    size_t getTaskCount() const; 
    void displayAllTasks() const;

};


#endif //TASKMANAGER_H