#include "LearningEngine.hpp"

namespace exotic {
LearningEngine::LearningEngine(Graph& graph) : graph_(graph) {}
LearningResult LearningEngine::Learn(UUID nodeId, bool success) { LearningResult result; auto* node = graph_.FindNode(nodeId); if (!node) { result.updated = false; result.confidenceDelta = 0.0; result.message = "Node not found."; return result; } node->metadata["last_learning_result"] = success ? "success" : "failure"; node->metadata["learned"] = "true"; result.updated = true; result.confidenceDelta = success ? 0.1 : -0.1; result.message = success ? "Learning reinforced node." : "Learning weakened node."; return result; }
}
