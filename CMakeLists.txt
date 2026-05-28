cmake_minimum_required(VERSION 3.10)
project(todo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(todo
    src/main.cpp
    src/tasks/task.cpp
    src/tasks/recurring_task.cpp
    src/tasks/timed_task.cpp
    src/taskManager.cpp
    src/taskStorage.cpp
)

target_include_directories(todo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Cross-platform UUID library linking
if(!APPLE)
    target_link_libraries(todo PRIVATE uuid)
endif()

