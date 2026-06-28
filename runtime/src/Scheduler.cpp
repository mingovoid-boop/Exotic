#include "Scheduler.hpp"

namespace exotic {

bool Scheduler::TaskCompare::operator()(const Task& a, const Task& b) const {
    return static_cast<int>(a.Priority()) < static_cast<int>(b.Priority());
}

void Scheduler::Queue(Task task) {
    tasks_.push(std::move(task));
}

void Scheduler::Execute() {
    while (!tasks_.empty()) {
        tasks_.top().Execute();
        tasks_.pop();
    }
}

void Scheduler::Clear() {
    while (!tasks_.empty()) {
        tasks_.pop();
    }
}

bool Scheduler::Empty() const {
    return tasks_.empty();
}

std::size_t Scheduler::Count() const {
    return tasks_.size();
}

}
