#pragma once

#include "Types.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace exotic {
struct SDKEvent {
    String name;
    std::unordered_map<String,String> data;
};

class SDKEventBus {
public:
    using Callback = std::function<void(const SDKEvent&)>;
    void Subscribe(const String& eventName, Callback callback);
    void Publish(const SDKEvent& event);
private:
    std::unordered_map<String,std::vector<Callback>> subscribers_;
};
}

