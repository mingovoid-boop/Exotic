#pragma once

#include "Graph.hpp"
#include "Types.hpp"

namespace exotic {
struct ExecutionResult {
    bool success = false;
    String message;
};

class ExecutionEngine {
public:
    explicit ExecutionEngine(Graph& graph);
    ExecutionResult Execute(UUID nodeId);
private:
    Graph& graph_;
};
}

