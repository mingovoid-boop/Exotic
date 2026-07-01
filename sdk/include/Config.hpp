#pragma once

#include "Types.hpp"
#include <unordered_map>

namespace exotic {
class Config {
public:
    void Set(const String& key, const String& value) { values_[key] = value; }
    String Get(const String& key, const String& fallback = "") const { auto it = values_.find(key); return it == values_.end() ? fallback : it->second; }
    bool Has(const String& key) const { return values_.find(key) != values_.end(); }
private:
    std::unordered_map<String,String> values_;
};
}

