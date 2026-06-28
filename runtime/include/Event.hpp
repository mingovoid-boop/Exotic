#pragma once

#include <chrono>
#include <string>

namespace exotic {

enum class EventType {
    Unknown,
    Runtime,
    Task,
    Plugin,
    State,
    Custom
};

class Event {
public:
    Event(std::string name, EventType type = EventType::Custom);

    const std::string& Name() const;
    EventType Type() const;
    std::chrono::steady_clock::time_point Time() const;

private:
    std::string name_;
    EventType type_;
    std::chrono::steady_clock::time_point time_;
};

}
