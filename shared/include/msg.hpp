#pragma once
#include "task.h"
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstdint>

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
            ss << task->serialize() << "\n";
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
            auto task = Task::deserialize(line);
            if (!task) throw std::runtime_error("Invalid task format");
            tasks.push_back(std::move(task));
        }

        return Msg{type, std::move(tasks)};
    }
};
