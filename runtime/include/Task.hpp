#pragma once

#include <functional>
#include <string>

namespace exotic {

enum class TaskPriority {
    Low,
    Normal,
    High,
    Critical
};

class Task {
public:
    using Function = std::function<void()>;

    Task(std::string name, Function function, TaskPriority priority = TaskPriority::Normal);

    void Execute() const;

    const std::string& Name() const;
    TaskPriority Priority() const;

private:
    std::string name_;
    Function function_;
    TaskPriority priority_;
};

}

