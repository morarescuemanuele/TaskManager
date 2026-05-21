#pragma once
#include "task.h"
#include "utils.h"
#include <string>

class TaskFactory {
public:
    enum class TaskType {
        Simple,
        Deadline,
        Recurring,
        Category
    };
    // Metoda statica ce returneaza un pointer la un obiect nou Task (Pattern Factory)
    static Task* createTask(TaskType type, const std::string& title, const std::string& description, const std::string& dateStr = "", int intervalSeconds = 0);
};
