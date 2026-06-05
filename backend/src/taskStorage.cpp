#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "taskStorage.h"

// CONSTRUCTOR
taskStorage::taskStorage(const std::filesystem::path& path) : storagePath(path) {} 

// SERIALIZATION 
std::string taskStorage::serializeTask(const Task& task) const {
    std::stringstream ss;
    
    auto deadlineTimeT = std::chrono::system_clock::to_time_t(task.getDeadline());

    ss << task.getId() << "|"
       << task.getTitle() << "|"
       << task.getDescription() << "|"
       << deadlineTimeT << "|"
       << static_cast<int>(task.getPriority()) << "|"
       << static_cast<int>(task.getRecurrence()) << "|"
       << (task.isCompleted() ? "1" : "0");

    return ss.str();
}

// DESERIALIZATION
std::unique_ptr<Task> taskStorage::deserializeTask(const std::string& line) const {
    std::stringstream ss(line);
    std::string id, title, description, deadlineStr, priorityStr, recurrenceStr, completedStr;

    if (!std::getline(ss, id, '|') ||
        !std::getline(ss, title, '|') ||
        !std::getline(ss, description, '|') ||
        !std::getline(ss, deadlineStr, '|') ||
        !std::getline(ss, priorityStr, '|') ||
        !std::getline(ss, recurrenceStr, '|') ||
        !std::getline(ss, completedStr)) {
        return nullptr;
    }

    std::time_t deadlineTime = std::stoll(deadlineStr);
    auto deadline = std::chrono::system_clock::from_time_t(deadlineTime);
    Priority priority = static_cast<Priority>(std::stoi(priorityStr));
    Recurrence recurrence = static_cast<Recurrence>(std::stoi(recurrenceStr));
    bool completed = (completedStr == "1");

    auto task = std::make_unique<Task>(title, deadline, priority, Category::OTHER, recurrence);
    task->setId(id);
    task->setDescription(description);
    if (completed) task->setCompleted(true);

    return task;
}

// DISK OPERATIONS 

// A) SAVE
bool taskStorage::saveTasks(const std::vector<std::unique_ptr<Task>>& tasks) const {
    try {
        if (storagePath.has_parent_path()) {
            std::filesystem::create_directories(storagePath.parent_path());
        }

        std::ofstream outFile(storagePath);
        if (!outFile.is_open()) return false;

        for (const auto& task : tasks) {
            if (task) {
                outFile << serializeTask(*task) << "\n";
            }
        }
        
        outFile.close();
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem Error while saving: " << e.what() << "\n";
        return false;
    }
}

// add Task
bool taskStorage::addTask(const std::unique_ptr<Task>& task) const {
    try {
        if (!task) return false;

        if (storagePath.has_parent_path()) {
            std::filesystem::create_directories(storagePath.parent_path());
        }

        std::ofstream outFile(storagePath, std::ios::app); // always writes to the end of the file
        if (!outFile.is_open()) return false;

        outFile << serializeTask(*task) << "\n";
        outFile.close();
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem Error while saving: " << e.what() << "\n";
        return false;
    }
}

// Update
bool taskStorage::updateTask(std::unique_ptr<Task>& task) {
     // load tasks
    std::vector<std::unique_ptr<Task>> tasks;
    if (!loadTasks(tasks)) return false;

    // find task to be updated
    auto it = std::find_if(tasks.begin(), tasks.end(), [&](const std::unique_ptr<Task>& t) {
        return t->getId() == task->getId();
    });
    if (it == tasks.end()) return false;

    // update and save
    *it = std::move(task);
    return saveTasks(tasks);
}

// Delete
bool taskStorage::deleteTask(std::unique_ptr<Task>& task) {
    // load tasks
    std::vector<std::unique_ptr<Task>> tasks;
    if (!loadTasks(tasks)) return false;

    // Claude: how to remove an elemnt form a vecotr based on member variable:
    tasks.erase(
        // move matching element to the end
        std::remove_if(tasks.begin(), tasks.end(), [&](const std::unique_ptr<Task>& t) {
            return t->getId() == task->getId();
        }),
        // remove last element
        tasks.end()
    );

    // save
    return saveTasks(tasks);
}

// B) LOAD
bool taskStorage::loadTasks(std::vector<std::unique_ptr<Task>>& tasks) {
    if (!std::filesystem::exists(storagePath)) {
        return false; 
    }

    std::ifstream inFile(storagePath);
    if (!inFile.is_open()) return false;

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        std::unique_ptr<Task> task = deserializeTask(line);
        if (task) {
            tasks.push_back(std::move(task));
        }
    }

    inFile.close();
    return true;
}

// load task
bool taskStorage::loadTask(std::unique_ptr<Task>& task) {
    // load tasks
    std::vector<std::unique_ptr<Task>> tasks;
    if (!loadTasks(tasks)) return false;

    // find task
    auto it = std::find_if(tasks.begin(), tasks.end(), [&](const std::unique_ptr<Task>& t) {
        return t->getId() == task->getId();
    });
    if (it == tasks.end()) return false;

    // return task
    task = std::move(*it);
    return true;
}
