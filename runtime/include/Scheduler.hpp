#pragma once

#include "Task.hpp"

#include <queue>
#include <vector>

namespace exotic {

class Scheduler {
public:
    void Queue(Task task);
    void Execute();
    void Clear();

    bool Empty() const;
    std::size_t Count() const;

private:
    struct TaskCompare {
        bool operator()(const Task& a, const Task& b) const;
    };

    std::priority_queue<Task, std::vector<Task>, TaskCompare> tasks_;
};

}

