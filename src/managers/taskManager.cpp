#include<fstream>
#include <algorithm>


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





// here algorithms -- > searching, sorting, filtering 


// here utilities 

// the end ..!?