#pragma once

#include "UUID.hpp"
#include "Types.hpp"

namespace exotic {
enum class EdgeType { Generic, Parent, Child, Reference, Dependency, Observation, Memory, Prediction, Alignment, Execution, Learning, Relationship };
struct GraphEdge {
UUID id = UUIDGenerator::Next();
UUID from = 0;
UUID to = 0;
EdgeType type = EdgeType::Generic;
f64 weight = 1.0;
bool directed = true;
};
}

