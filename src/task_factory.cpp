#include "../include/task_factory.h"

Task* TaskFactory::createTask(TaskType type, const std::string& title, const std::string& description, const std::string& dateStr, int intervalSeconds) 
{
    switch (type) {
        case TaskType::Simple:
            return new SimpleTask(title, description);
        case TaskType::Deadline:
            return new DeadlineTask(title, description, TimeUtils::citesteData(dateStr));
        case TaskType::Recurring:
            return new RecurringTask(title, description, 
                                     TimeUtils::citesteData(dateStr), 
                                     Duration(intervalSeconds));
        case TaskType::Category:
            return new Category(title, description);

        default:
            return nullptr;
    }
}
