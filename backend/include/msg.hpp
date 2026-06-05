#pragma once
#include "tasks/task.h"
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <stdexcept>

enum Type {
    ADD,
    GET,
    GETALL,
    UPDATE,
    DELETE,
    NOTIFY,
    ERROR,
};

struct Msg {
    Type type;
    std::vector<std::unique_ptr<Task>> tasks;

    // Claude: add (de)serialization to std::vector<uint8_t>:
    std::vector<uint8_t> serialize() const {
        std::stringstream ss;
        ss << static_cast<int>(type) << "\n";
        for (const auto& task : tasks) {
            if (!task) continue;
            auto deadlineTimeT = std::chrono::system_clock::to_time_t(task->getDeadline());
            ss << task->getId() << "|"
               << task->getTitle() << "|"
               << task->getDescription() << "|"
               << deadlineTimeT << "|"
               << static_cast<int>(task->getPriority()) << "|"
               << static_cast<int>(task->getRecurrence()) << "|"
               << (task->isCompleted() ? "1" : "0") << "\n";
        }
        std::string s = ss.str();
        return std::vector<uint8_t>(s.begin(), s.end());
    }

    static Msg deserialize(const std::vector<uint8_t>& data) {
        std::string str(data.begin(), data.end());
        std::stringstream ss(str);
        std::string line;

        if (!std::getline(ss, line))
            throw std::runtime_error("Invalid message format");
        Type type = static_cast<Type>(std::stoi(line));

        std::vector<std::unique_ptr<Task>> tasks;
        while (std::getline(ss, line)) {
            if (line.empty()) continue;
            std::stringstream ls(line);
            std::string id, title, description, deadlineStr, priorityStr, recurrenceStr, completedStr;
            if (!std::getline(ls, id, '|') ||
                !std::getline(ls, title, '|') ||
                !std::getline(ls, description, '|') ||
                !std::getline(ls, deadlineStr, '|') ||
                !std::getline(ls, priorityStr, '|') ||
                !std::getline(ls, recurrenceStr, '|') ||
                !std::getline(ls, completedStr))
                throw std::runtime_error("Invalid task format");

            auto deadline = std::chrono::system_clock::from_time_t(std::stoll(deadlineStr));
            Priority priority = static_cast<Priority>(std::stoi(priorityStr));
            Recurrence recurrence = static_cast<Recurrence>(std::stoi(recurrenceStr));

            auto task = std::make_unique<Task>(title, deadline, priority, Category::OTHER, recurrence);
            task->setId(id);
            task->setDescription(description);
            if (completedStr == "1") task->setCompleted(true);
            tasks.push_back(std::move(task));
        }

        return Msg{type, std::move(tasks)};
    }
};
