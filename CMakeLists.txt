cmake_minimum_required(VERSION 3.10)
project(todo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(todo
    src/main.cpp
    src/tasks/task.cpp
    src/tasks/recurring_task.cpp
    src/tasks/timed_task.cpp
    src/managers/taskManager.cpp
    src/storage/taskStorage.cpp
)

