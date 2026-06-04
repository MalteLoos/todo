// Planned task operations: add - delete - update - finish - search - filter -
// sort

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "taskStorage.h" // => taskManager uses storage for persistence
#include "tasks/task.h"  // => taskManager relies on Task definition

enum class FilterMode {
  ALL,
  PRIO_LOW,
  PRIO_MED,
  PRIO_HIGH,
  COMPLETED,
  OVERDUE,
  RECURRING,
  TIMED
};
enum class SortMode { DEADLINE, PRIOITY };

class TaskManager {
private:
  std::vector<std::unique_ptr<Task>> tasks;
  taskStorage storage; // => handles file I/O

public:
  // CONSTRUCTOR
  TaskManager() = default;

  void addTask(std::unique_ptr<Task> tasks);
  bool deleteTask(const std::string &id);
  bool updateTask(const std::string &id, std::unique_ptr<Task> updatedTask);
  bool finishTask(const std::string &id);

  // STORAGE
  void saveTasks() const; // => saves all tasks to disk
  void loadTasks();       // => load tasks from disk into memory

  // ALGORITHMS
  Task *searchById(const std::string &id);

  std::vector<Task *> searchByTitle(const std::string &keyword)
      const; // => searching tasks whose title contains a specific >> keyword<<

  std::vector<Task *> filterByPriority(
      Priority priority) const; // => filtering tasks based on priority leve
  std::vector<Task *> filterByStatus(bool completed)
      const; // => filtering tasks aka. get the completed or incomplted ones
  std::vector<Task *>
  getOverdueTasks() const; // => retrieve tasks past deadline

  std::vector<Task *>
  getTasksSortedByDeadline() const; // => sorting tasks by their deadlines
  void getTasksSortedByDeadline(std::vector<Task *> &sortedTasks) const;
  std::vector<Task *>
  getTasksSortedByPriority() const; // => sort: highest to lowest
  void getTasksSortedByPriority(std::vector<Task *> &sortedTasks) const;

  std::vector<Task *>
  getRecurringTasks() const;                 // => retrieve all repeated tasks
  std::vector<Task *> getTimedTasks() const; // => retrieved all tasks w/ timer

  std::vector<Task *> getTasksForView(const std::string &keyword,
                                      FilterMode filtermode,
                                      SortMode sortmode) const;

  // UTILITIES
  size_t getTaskCount() const;
  void displayAllTasks() const;
};

#endif // TASKMANAGER_H