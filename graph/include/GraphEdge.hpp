#pragma once

#include "UUID.hpp"
#include "Types.hpp"

namespace exotic {
enum class EdgeType { Generic, Parent, Child, Reference, Dependency, Observation, Memory, Prediction, Alignment, Execution, Learning, Relationship };
