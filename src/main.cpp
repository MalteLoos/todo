#include <iostream>
#include <memory>
#include <chrono>

#include "managers/taskManager.h"
#include "tasks/task.h"
#include "tasks/timed_task.h"
#include "tasks/recurring_task.h"
#include "storage/taskStorage.h"

int main() {
    TaskManager manager; // => taskManager instance 

    // TIME SPECIFICTIONS 
    auto now = std::chrono::system_clock::now();
    auto tomorrow = std::chrono::system_clock::hours(24);
    auto nextWeek = std::chrono::system_clock::hours(24 * 7);

    //CREATING TASKS
    auto taskA = std::make_unique<Task>("Finish C++ Project", tomorrow, Priority::HIGH, Category::EDUCATION, Recurrence::NONE);
    taskA -> setDescription("Final debugging and testing, then submit.");
    manager.addTask(std::move(taskA));

}
