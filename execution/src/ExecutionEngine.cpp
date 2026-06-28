#include "ExecutionEngine.hpp"

namespace exotic {
ExecutionEngine::ExecutionEngine(Graph& graph) : graph_(graph) {}

ExecutionResult ExecutionEngine::Execute(UUID nodeId) {
    ExecutionResult result;
    auto* node = graph_.FindNode(nodeId);
    if (!node) {
        result.success = false;
        result.message = "Node not found.";
        return result;
    }
    node->metadata["last_execution"] = "completed";
    result.success = true;
    result.message = "Execution completed.";
    return result;
}

}
