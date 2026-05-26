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

    auto taskB = std::make_unique<Task>(
        "Go Gym session", 
        now + std::chrono::hours(2), 
        Priority::LOW, 
        Category::PERSONAL, 
        Recurrence::DAILY
    );
    taskB->setDescription("Cardio Routine");
    manager.addTask(std::move(taskB));

    // A) TIMED TASK 
    auto timedTask = std::make_unique<TimedTask>("C++ Homework Assignment",nextWeek, Priority::MEDIUM, Recurrence::WEEKLY, now, std::chrono::minutes(90));
    timedTask -> setDescription("Complete 2 out of 4 exercises");
    manager.addTask(std::move(timedTask));

    // B) RECURRING TASK
    auto recurringTask = std::make_unique<RecurringTask>("Rent Payment", now + std::chrono::hours(24 * 30), Priority::HIGH, Recurrence::MONTHLY);
    recurringTask -> setDescrioption("Transfer 500€ to landlord");
    manager.addTask(std::move(recurringTask));

    
    

}
