#include "Task.hpp"

#include <utility>

namespace exotic {

Task::Task(std::string name, Function function, TaskPriority priority)
    : name_(std::move(name)), function_(std::move(function)), priority_(priority) {}

void Task::Execute() const {
    if (function_) {
        function_();
    }
}

const std::string& Task::Name() const {
    return name_;
}

TaskPriority Task::Priority() const {
    return priority_;
}

}
