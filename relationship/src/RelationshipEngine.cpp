#include "RelationshipEngine.hpp"

namespace exotic {
RelationshipEngine::RelationshipEngine(Graph& graph) : graph_(graph) {}
UUID RelationshipEngine::Connect(UUID from, UUID to, EdgeType type, f64 weight) { GraphEdge edge; edge.from = from; edge.to = to; edge.type = type; edge.weight = weight; return graph_.AddEdge(edge); }
bool RelationshipEngine::Exists(UUID edgeId) const { return graph_.HasEdge(edgeId); }
}
