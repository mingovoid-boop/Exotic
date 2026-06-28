#include "EventBus.hpp"

namespace exotic {

void EventBus::Subscribe(EventType type, Handler handler) {
    handlers_[type].push_back(std::move(handler));
}

void EventBus::Publish(const Event& event) const {
    auto it = handlers_.find(event.Type());
    if (it == handlers_.end()) {
        return;
    }

    for (const auto& handler : it->second) {
        if (handler) {
            handler(event);
        }
    }
}

void EventBus::Clear() {
    handlers_.clear();
}

}
