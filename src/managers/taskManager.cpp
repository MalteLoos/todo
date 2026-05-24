#include<fstream>
#include <algorithm>
#include <iostream>

#include "taskManager.h"

// OPERATIONS UPON TASKS
void TaskManager::addTask(const Task& task){
    tasks[task.getId()] = task;
}

bool TaskManager::deleteTask(const std::string& id){
    return tasks.erase(id) > 0; // => erase =[0, 1] no. of tasks deleted
}

bool TaskManager::updateTask(const std::string& id, const Task& updatedTask){
    auto iterator = tasks.find(id); // => task with given id was:
    if (iterator != tasks.end()){
        iterator->second = updatedTask;
        return true; // => a) found
    } 
    return false; // => b) not found
}

bool TaskManager::finishTask(const std::string& id){
    auto iterator = tasks.find(id);
    if (iterator != tasks.end()){
        iterator -> second.setCompleted(true);
        return true;
    
    }
    return false;
}

// SEARCH ALGORITHM
Task* TaskManager::searchById(const std::string& id){
    auto iterator = tasks.find(id);
    if (iterator != tasks.end()){ 
        return &(iterator -> second);
    
    }
    return nullptr;
}

std::vector<Task> TaskManager::searchByTitle(const std::string& keyword)const{
    std::vector<Task> result; // => to store matching tasks
    for (const auto& pair : tasks){ // => pair: <id and task>
        if (pair.second.getTitle().find(keyword) != std::string::npos){ // => case: keyword in title found
            result.push_back(pair.second);
        }
    }
    return result;    
}

// FILTERING ALGORITHMS
std::vector<Task> TaskManager::filterByPriority(Priority priority) const{
    std::vector<Task> result;
    for (const auto& pair : tasks){
        if (pair.second.getPriority() == priority){
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Task> TaskManager::filterByStatus(bool completed) const{
    std::vector<Task> result;
    for (const auto& pair : tasks){
        if (pair.second.isCompleted() == completed){
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Task> TaskManager::getOverdueTasks() const{
    std::vector<Task> result;
    for (const auto& pair : tasks){
        if (pair.second.isOverdue()){
            result.push_back(pair.second);
        }
    }
    return result;
}

//SORTING ALGORITHMS 
std::vector<Task> TaskManager::getTasksSortedByDeadline() const{
    std::vector<Task> sortedTasks; // => sorted tasks <-> result
    for (const auto& pair : tasks){
        sortedTasks.push_back(pair.second); // => copying tasks map to vector 
    }
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task& a, const Task& b){ 
        return a.getDeadline() < b.getDeadline(); // => lambda function to compare deadlines for sorting
    });
    return sortedTasks;
}

std::vector<Task> TaskManager::getTasksSortedByPriority() const{
    std::vector<Task> sortedTasks; 
    for (const auto& pair : tasks){
        sortedTasks.push_back(pair.second); 
    }
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task& a, const Task& b){ 
        return static_cast<int>(a.getPriority()) > static_cast<int>(b.getPriority());
    });
    return sortedTasks;
}

// UTILITIES
size_t TaskManager::getTaskCount() const{
    return tasks.size();
}

void TaskManager::displayAllTasks()const{
    if (tasks.empty()){
        std::cout << "There are no tasks available.\n";
        return;
    }
    std::cout << "All Tasks:\n";
    for(const auto& pair: tasks){
        std::cout << pair.second.toString() << "\n"); // => displaying task details 
        std::cout <<"-" * 40 << "\n"
}

