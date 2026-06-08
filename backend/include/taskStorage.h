#ifndef TASK_STORAGE_H
#define TASK_STORAGE_H

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include "task.h"

class taskStorage{
private:
    std::filesystem::path storagePath; // => path to the storage file (e.g., tasks.json)
    std::string serializeTask(const Task& task) const; // => convert a Task object to a JSON string
    std::unique_ptr<Task> deserializeTask(const std::string& line) const; // => convert a JSON

public: 
    // CONSTRUCTOR
    taskStorage(const std::filesystem::path& path = "data/tasks.json");
    bool saveTasks(const std::vector<std::unique_ptr<Task>>& tasks) const; // => save tasks to disk
    bool addTask(const std::unique_ptr<Task>& task) const; // => add task to disk
    bool updateTask(std::unique_ptr<Task>& task); // -> update task on disk
    bool deleteTask(std::unique_ptr<Task>& task); // -> delete task from disk
    bool loadTasks(std::vector<std::unique_ptr<Task>>& tasks); // => load tasks from disk
    bool loadTask(std::unique_ptr<Task>& task); // => load task from disk
};   

#endif //TASK_STORAGE_H