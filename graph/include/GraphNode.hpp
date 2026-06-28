#pragma once

#include "UUID.hpp"
#include "Types.hpp"
#include <unordered_map>
#include <vector>

namespace exotic {
struct GraphNode {
    UUID id = UUIDGenerator::Next();
    String name;
    std::vector<UUID> connections;
    std::unordered_map<String,String> metadata;
};
}
