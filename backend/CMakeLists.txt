# Claude: write the cmake file:

cmake_minimum_required(VERSION 3.16)
project(todo_backend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(todo_backend
    src/main.cpp
    src/backend.cpp
    src/taskStorage.cpp
    src/reminder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/recurring_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/timed_task.cpp
)

target_include_directories(todo_backend PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR}/../include)

find_package(Threads REQUIRED)
target_link_libraries(todo_backend PRIVATE Threads::Threads)

if(NOT APPLE)
    target_link_libraries(todo_backend PRIVATE uuid)
endif()
