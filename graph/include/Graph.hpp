#pragma once

#include "GraphNode.hpp"
#include "GraphEdge.hpp"
#include <unordered_map>

namespace exotic {
class Graph {
public:
UUID AddNode(const GraphNode& node);
UUID AddEdge(const GraphEdge& edge);
bool HasNode(UUID id) const;
bool HasEdge(UUID id) const;
GraphNode* FindNode(UUID id);
GraphEdge* FindEdge(UUID id);
const std::unordered_map<UUID, GraphNode>& Nodes() const;
const std::unordered_map<UUID, GraphEdge>& Edges() const;
private:
std::unordered_map<UUID, GraphNode> nodes_;
std::unordered_map<UUID, GraphEdge> edges_;
};
}

