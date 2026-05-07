# **todo** 
## Project Plan

*This workplan for a Smart To-Do Task and Task Manager is structured into three distinct parts, assigning one student to lead each core area: User Interface (Frontend), System Architecture (Backend), and Advanced Utilities (Concurrency & Smart Logic).*

## **Student 1:** GUI Developer & User Interaction
Role in the App Flow: This part serves as the entry point and visual interface of the application. It is responsible for capturing user input (task names, deadlines) and displaying the task lists to the user
. This student ensures that user actions, such as clicking an "Add Task" button, trigger the appropriate logic in the backend.


**==> The Pipeline:**

**Initialization:** Set up the QApplication instance to manage application resources

**Layout Design:** Organize the interface using QVBoxLayout or QGridLayout to ensure widgets remain positioned correctly when resized

**Widget Implementation:** Create QLineEdit for task entry, QListWidget for displaying tasks, and QPushButton for actions

**Signal/Slot Wiring:** Use the connect() function to link GUI events (like a button click) to backend processing slots

**Event Loop:** Start the message loop with app.exec() to keep the window responsive to user events

**==> Lecture Concepts Included:**

**Qt Framework:** QApplication, QWidget, and QPushButton

**Layout Management:** Vertical, horizontal, and grid layouts

**Event-Driven Programming:** The mechanism of signals and slots

**Specific Widgets:** QLineEdit, QComboBox, and QListWidget


--------------------------------------------------------------------------------
## **Student 2:** Core Architecture & Data Management
Role in the App Flow: This part acts as the "brain" and storage of the app. It defines what a "Task" object is, stores these tasks in efficient collections, and handles the logic for sorting, searching, and saving the data to a file so it persists after the app closes

**==> The Pipeline:**

**Class Design:** Create a Task class using OOP principles, including appropriate constructors, access specifiers, and member functions

**Container Selection:** Choose the best STL container (e.g., std::vector for simple lists or std::map for keyed lookup) to store task objects

**Algorithmic Logic:** Implement sorting (by priority or date) and searching using the STL Algorithms library

**Persistent Storage:** Use the std::filesystem library to implement save/load functionality, ensuring tasks are written to and read from disk

**==> Lecture Concepts Included:**

**Object-Oriented Programming:** Classes, structs, inheritance, and access specifiers (public/private)

**Constructors/Destructors:** Proper resource acquisition and release

**STL Containers:** std::vector, std::map, and std::array

**STL Algorithms:** std::sort, std::find_if, and std::remove_if

**Filesystem Library:** Path objects and directory iterators for data persistence


--------------------------------------------------------------------------------
## **Student 3:** Advanced Utilities & Concurrency

Role in the App Flow: This part provides performance optimization and "smart" features. This student is responsible for background tasks (like reminder notifications), thread safety to prevent data corruption, and modern C++ optimizations like move semantics and smart memory management to prevent leaks

 **==> The Pipeline:**

**Memory Management:** Wrap task pointers in std::unique_ptr or std::shared_ptr to automate memory deallocation

**Asynchronous Reminders:** Use std::thread and std::chrono to create a background timer that checks for upcoming deadlines without freezing the GUI

**Thread Safety:** Protect the shared task list with std::mutex and std::lock_guard to ensure Student 1's GUI and Student 3's background threads don't crash the app

**Smart Filtering:** Use Ranges and Views to create "smart" filtered lists (e.g., "Show only high priority tasks") without copying the original data

**==> Lecture Concepts Included:**

**Smart Pointers:** unique_ptr, shared_ptr, and preventing memory leaks

**Concurrency:** Multi-threading, std::thread, std::async, and std::future

**Synchronization:** std::mutex and thread-safe data access

**Modern Features:** Lambda expressions for inline callbacks, Move semantics (std::move), and Ranges/Views

**Time Utilities:** std::chrono for measuring time and handling deadlines
