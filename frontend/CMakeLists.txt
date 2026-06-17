cmake_minimum_required(VERSION 3.16)
project(todo_frontend)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Always generate compile_commands.json for IDE support (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure Qt 6 path (based on local installation, wrapped in EXISTS for cross-platform portability)
if(EXISTS "/Users/daniel/Qt/6.11.1/macos")
    list(APPEND CMAKE_PREFIX_PATH "/Users/daniel/Qt/6.11.1/macos")
endif()

# Enable automatic compilation of Qt Meta-Objects (moc), UI files (uic), and Resources (rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 Components
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Console executable
#add_executable(todo
#    src/main.cpp
#    src/tasks/task.cpp
#    src/tasks/recurring_task.cpp
#    src/tasks/timed_task.cpp
#    src/taskManager.cpp
#)
#
#target_include_directories(todo PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/include
#)

# Cross-platform UUID library linking
#if(NOT APPLE)
#    target_link_libraries(todo PRIVATE uuid)
#endif()

# GUI executable
add_executable(todo_gui
    src/mainGUI.cpp
    src/client.cpp
    src/taskManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/src/tasks/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/src/tasks/recurring_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/src/tasks/timed_task.cpp
)

target_include_directories(todo_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared/include
)

find_package(Threads REQUIRED)
target_link_libraries(todo_gui PRIVATE Qt6::Widgets Threads::Threads)

# Cross-platform UUID library linking
if(NOT APPLE)
    target_link_libraries(todo_gui PRIVATE uuid)
endif()

