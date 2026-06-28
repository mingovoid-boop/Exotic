#pragma once

#include "Event.hpp"

#include <functional>
#include <unordered_map>
#include <vector>

namespace exotic {

class EventBus {
public:
    using Handler = std::function<void(const Event&)>;

    void Subscribe(EventType type, Handler handler);
    void Publish(const Event& event) const;
    void Clear();

private:
    std::unordered_map<EventType, std::vector<Handler>> handlers_;
};

}
