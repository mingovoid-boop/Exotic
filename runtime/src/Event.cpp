#include "Event.hpp"

#include <utility>

namespace exotic {

Event::Event(std::string name, EventType type)
    : name_(std::move(name)), type_(type), time_(std::chrono::steady_clock::now()) {}

const std::string& Event::Name() const {
    return name_;
}

EventType Event::Type() const {
    return type_;
}

std::chrono::steady_clock::time_point Event::Time() const {
    return time_;
}

}
