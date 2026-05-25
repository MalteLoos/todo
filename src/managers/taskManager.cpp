#include<fstream>
#include <algorithm>
#include <iostream>
#include <memory>

#include "taskManager.h"
#include "../tasks/timed_task.h"

// SUGGESTION ... mutex locing/protection ...?

// OPERATIONS UPON TASKS
void TaskManager::addTask(std::unique_ptr<Task> task){
    tasks.push_back(std::move(task)); // => adding new task to the vector
    }

bool TaskManager::deleteTask(const std::string& id){
        auto iterator = std::remove_if(tasks.begin(),tasks.end(),
        [&id](const std::unique_ptr<Task>& task){ // => lambda function finding task by id 
            return task->getId() == id;
        }
    );
    if(iterator != tasks.end()){

        tasks.erase(iterator, tasks.end());

        return true;
    }

    return false;
}

bool TaskManager::updateTask(const std::string& id, const Task& updatedTask){
    auto iterator = std::find_if(tasks.begin(), tasks.end(), [&id](const std::unique_ptr<Task>& task){
        return task->getId() == id;
    });
    if (iterator != tasks.end()){
        *iterator = std::make_unique<Task>(updatedTask);

        // (*iterator)->setTitle(updatedTask.getTitle());
        // (*iterator)->setDescription(updatedTask.getDescription());
        // (*iterator)->setPriority(updatedTask.getPriority());
        // (*iterator)->setRecurrence(updatedTask.getRecurrence());
        // (*iterator)->setCategory(updatedTask.getCategory());
        return true; // => a) found and updated
    }
    return false; // => b) not found
}

bool TaskManager::finishTask(const std::string& id){
    auto iterator = std::find_if(tasks.begin(), tasks.end(), [&id](const std::unique_ptr<Task>& task){
        return task->getId() == id;
    });
    if(iterator != tasks.end()){
        (*iterator)->setCompleted(true);
        return true;
    }
    return false;
}

// SEARCH ALGORITHM
Task* TaskManager::searchById(const std::string& id){
    for (const auto& task : tasks){
        if (task ->getId() == id){
            return task.get(); // => returning raw pointer
        }
    }
    return nullptr;
}

std::vector<Task*> TaskManager::searchByTitle(const std::string& keyword)const{
    std::vector<Task*> result; // => to be changed - case sensitive
    
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower); // => converting all to lowercase
    for(const auto& task : tasks){
        std::string lowerTitle = task ->getTitle();
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower); 
        if (lowerTitle.find(lowerKeyword) !=std::string::npos){
            result.push_back(task.get());    
        }
    }
    return result;    
}

// FILTERING ALGORITHMS
std::vector<Task*> TaskManager::filterByPriority(Priority priority) const{
    std::vector<Task*> result;
    for (const auto& task : tasks){ 
        if (task -> getPriority() == priority){
            result.push_back(task.get());
        }
    }
    return result;
}

std::vector<Task*> TaskManager::filterByStatus(bool completed) const{
    std::vector<Task*> result;
    for (const auto& task : tasks){ 
        if (task -> isCompleted() == completed){
            result.push_back(task.get());
        }
    }
    return result;
}

std::vector<Task*> TaskManager::getOverdueTasks() const{
    std::vector<Task*> result;
    for (const auto& task : tasks){
        if (task -> isOverdue()){
            result.push_back(task.get());
        }
    }
    return result;
}

//SORTING ALGORITHMS 
std::vector<Task*> TaskManager::getTasksSortedByDeadline() const{
    std::vector<Task*> sortedTasks; // => sorted tasks <-> result
    for (const auto& task : tasks){
        sortedTasks.push_back(task.get()); // => copying tasks map to vector 
    }
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task* a, const Task* b){ 
        return a -> getDeadline() < b -> getDeadline(); // => lambda function to compare deadlines for sorting
    });
    return sortedTasks;
}

std::vector<Task*> TaskManager::getTasksSortedByPriority() const{
    std::vector<Task*> sortedTasks; 
    for (const auto& task : tasks){
        sortedTasks.push_back(task.get()); 
    }
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task* a, const Task* b){ 
        return static_cast<int>(a -> getPriority()) > static_cast<int>(b -> getPriority());
    });
    return sortedTasks;
}

std::vector<Task*> TaskManager::getRecurringTasks() const{
    std::vector<Task*> result;
    for (const auto& task : tasks){
        if (task->getRecurrence() != Recurrence::NONE){
            result.push_back(task.get());
        }
    }
    return result;
}

std::vector<Task*> TaskManager::getTimedTasks() const{
    std::vector<Task*> result;
    for (const auto& task : tasks){
        if (task ->getType() == "timedTask")
            result.push_back(task.get());
        }
    }
    return result;
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
    for(const auto& task : tasks){
        std::cout << task->toString()<< "\n";
        std::cout << std::string(40, '-') << "\n";
    }
}

