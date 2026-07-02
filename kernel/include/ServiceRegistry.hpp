#pragma once

#include "Types.hpp"
#include <unordered_map>

namespace exotic {

class ServiceRegistry {
public:
    void Register(const String& name, void* service) {
        services[name] = service;
    }

    void* Get(const String& name) const {
        auto it = services.find(name);
        return it == services.end() ? nullptr : it->second;
    }

private:
    std::unordered_map<String, void*> services;
};

} // namespace exotic