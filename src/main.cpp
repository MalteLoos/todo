#include <iostream>
#include <memory>
#include <chrono>

#include "taskManager.h"
#include "tasks/task.h"
#include "tasks/timed_task.h"
#include "tasks/recurring_task.h"
#include "taskStorage.h"

int main() {
    TaskManager manager; // => taskManager instance 

    // TIME SPECIFICTIONS 
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    auto nextWeek = now + std::chrono::hours(24 * 7);

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
    auto timedTaskObj = std::make_unique<timedTask>("C++ Homework Assignment",nextWeek, Priority::MEDIUM, Recurrence::WEEKLY, now, std::chrono::minutes(90));
    timedTaskObj -> setDescription("Complete 2 out of 4 exercises");
    manager.addTask(std::move(timedTaskObj));

    // B) RECURRING TASK
    auto recurringTaskObj = std::make_unique<recurringTask>("Rent Payment", now + std::chrono::hours(24 * 30), Priority::HIGH, Recurrence::MONTHLY);
    recurringTaskObj -> setDescription("Transfer 500€ to landlord");
    manager.addTask(std::move(recurringTaskObj));

    // DISPLAYING ALL TASKS (number)
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "                  ALL CURRENT TASKS            \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    std::cout << "\n Total Tasks: " << manager.getTaskCount() << "\n\n";

    // SEARCHING 
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "               SEARCH RESULTS: >GYM<           \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto searchResults = manager.searchByTitle("Gym");
    for(const auto* task : searchResults){

    std::cout << task->toString() << "\n";
}
    // FILTERING
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "               HIGH PRIORITY TASKS             \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto highPriorityTasks = manager.filterByPriority(Priority::HIGH);
    for (const auto* task : highPriorityTasks){
        std::cout << task -> toString() << "\n\n";    
}
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "               INCOMPLETE TASKS                \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto incompleteTasks = manager.filterByStatus(false);
    for (const auto* task : incompleteTasks){
        std::cout << task -> toString() << "\n\n";
}

    // SORTING 
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "             SORTING BY PRIORITY               \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto sortedByPriority = manager.getTasksSortedByPriority();
    for (const auto* task : sortedByPriority){
        std::cout << task -> toString() << "\n\n";
    }

    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "             SORTING BY DEADLINE               \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto sortedByDeadline = manager.getTasksSortedByDeadline();
    for (const auto* task : sortedByDeadline){
        std::cout << task -> toString() << "\n\n";
    }

    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "                      LATE TASKS               \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto lateTasks = manager.getOverdueTasks();
    for (const auto* task : lateTasks){
        std::cout << task -> toString() << "\n\n";
    }

    // MARK AS FINISHED
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "                    FINISH TASKS               \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    auto gymResults = manager.searchByTitle("Gym");
    if (!gymResults.empty()) {
        Task* taskToBeFinished = gymResults[0];
        manager.finishTask(taskToBeFinished->getId());
        std::cout << "Task '" << taskToBeFinished->getTitle() << "' marked as completed.\n";
    } else {
        std::cout << "Task Gym was not found!\n";
    }

    // DISPLAYING ALL TASKS (updated)
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "                  ALL CURRENT TASKS            \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    manager.displayAllTasks();

    // SAVING TO THE DISK
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "                 SAVING TASKS                  \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    manager.saveTasks();
    std::cout << "Tasks saved successfully.\n";

    // LOADING FROM THE DISK
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "               LOADING TASKS                   \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";


    TaskManager loadedManager;

    loadedManager.loadTasks();

    // DISPLAY ALL TASKS (final)
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "                LOADED TASKS                   \n";
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    loadedManager.displayAllTasks();

    std::cout << "\n sucesful execution ...\n";
    
    return 0;
}

