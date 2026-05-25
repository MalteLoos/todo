#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>

#include"task.h" // => class definition 
#include <uuid/uuid.h> // => generating unique identifiers

static std::string generateID(){  // => generating a unique identifier
    uuid_t uuid;
    uuid_generate(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}
// CONVERTING TO STRING 
static std::string priorityToString(Priority priority){
    switch(priority){
        case Priority::LOW: 
            return "LOW";
        case Priority::MEDIUM:
            return "MEDIUM";
        case Priority::HIGH:
            return "HIGH";
        default: 
            return "UNKNOWN";
    }
}
static std::string recurrenceToString(Recurrence recurrence){
    switch(recurrence){
        case Recurrence::NONE: 
            return "NONE";
        case Recurrence::DAILY:
            return "DAILY";
        case Recurrence::WEEKLY:
            return "WEEKLY";
        case Recurrence::MONTHLY:
            return "MONTHLY";
        case Recurrence::YEARLY:
            return "YEARLY";
        default: 
            return "UNKNOWN";
    }
}

static std::string timePointToString(const std::chrono::system_clock::time_point& tp){
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
    return ss.str();
}

// CONSTRUCTORS
Task::Task():
    id(generateID()),
    title("Untitled Task"),
    description(""),
    deadline(std::chrono::system_clock::now()),
    priority(Priority::LOW),
    recurrence(Recurrence::NONE),
    category(Category::OTHER),
    completed(false){}

Task::Task(const std::string& title,
           const std::chrono::system_clock::time_point& deadline,
           Priority priority, 
           Category category,
           Recurrence recurrence):
           id(generateID()),
           title(title),
           description(""),
           deadline(deadline),
           priority(priority),
           category(category),
           recurrence(recurrence),
           completed(false){}

// GETTERS
std::string Task::getId() const{
    return id;
}
std::string Task::getTitle() const{
    return title;
}
std::string Task::getDescription() const{
    return description;
}
std::chrono::system_clock::time_point Task::getDeadline() const{
    return deadline;
}
Priority Task::getPriority() const{
    return priority;
}
Recurrence Task::getRecurrence() const{
    return recurrence;
}
std::vector<std::string> Task::getLabel() const{
    return label;
}

//SETTERS
void Task:: setTitle(const std::string& title){
    this -> title = title;
}
void Task::setDescription(const std::string& description){
    this -> description = description; 
}
void Task::setPriority(Priority priority){
    this -> priority = priority;
}
void Task::setRecurrence(Recurrence recurrence){
    this -> recurrence = recurrence;
}
void Task::setLabel(const std::string& label){
    this -> label.push_back(label);
}

//UTILITES
bool Task::isOverdue() const{
    return std::chrono::system_clock::now() > deadline && !completed;
}


std::string Task::toString() const{
    std::stringstream ss;
    ss << "Task ID: " << id << "\n";
    ss << "Title: " << title << "\n";
    ss << "Description: " << description << "\n";
    ss << "Deadline: " << timePointToString(deadline) << "\n";
    ss << "Priority: " << priorityToString(priority) <<  "\n";
    ss << "Recurrence: " << recurrenceToString(recurrence) << "\n";
    ss << "Completed: " << (completed ? "Yes" : "No") << "\n";
    ss << "Labels: ";
    for(const auto& lab: label){
        ss << "#" << lab << " ";
    }  
    return ss.str();
}

Category Task::getCategory() const {
    return category;
}

void Task::setCategory(Category cat) {
    category = cat;
}

bool Task::isCompleted() const {
    return completed;
}

void Task::setCompleted(bool comp) {
    completed = comp;
}

// Helper to convert time_point to string
std::string Task::timePointToString(const std::chrono::system_clock::time_point& tp) const {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M");
    return ss.str();
}