#pragma once

#include "Graph.hpp"
#include "Types.hpp"

namespace exotic {
class RelationshipEngine {
public:
    explicit RelationshipEngine(Graph& graph);
    UUID Connect(UUID from, UUID to, EdgeType type = EdgeType::Relationship, f64 weight = 1.0);
    bool Exists(UUID edgeId) const;
private:
    Graph& graph_;
};
}

