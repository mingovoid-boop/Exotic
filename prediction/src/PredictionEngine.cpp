#include "PredictionEngine.hpp"

namespace exotic {
PredictionEngine::PredictionEngine(Graph& graph) : graph_(graph) {}
Prediction PredictionEngine::Predict(UUID nodeId) const { Prediction p; p.target = nodeId; if (graph_.HasNode(nodeId)) { p.confidence = 1.0; p.description = "Node exists and is stable."; } else { p.confidence = 0.0; p.description = "Node does not exist."; } return p; }
std::vector<Prediction> PredictionEngine::PredictNeighbors(UUID nodeId) const { std::vector<Prediction> results; auto node = const_cast<Graph&>(graph_).FindNode(nodeId); if (!node) return results; for (UUID id : node->connections) { results.push_back(Predict(id)); } return results; }
}
