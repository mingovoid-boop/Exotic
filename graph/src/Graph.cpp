#include "Graph.hpp"

namespace exotic {
UUID Graph::AddNode(const GraphNode& node) { nodes_[node.id]=node; return node.id; }
UUID Graph::AddEdge(const GraphEdge& edge) { edges_[edge.id]=edge; return edge.id; }
bool Graph::HasNode(UUID id) const { return nodes_.find(id) != nodes_.end(); }
bool Graph::HasEdge(UUID id) const { return edges_.find(id) != edges_.end(); }
GraphNode* Graph::FindNode(UUID id) { auto it=nodes_.find(id); return it==nodes_.end() ? nullptr : &it->second; }
GraphEdge* Graph::FindEdge(UUID id) { auto it=edges_.find(id); return it==edges_.end() ? nullptr : &it->second; }
const std::unordered_map<UUID,GraphNode>& Graph::Nodes() const { return nodes_; }
const std::unordered_map<UUID,GraphEdge>& Graph::Edges() const { return edges_; }
}
