#pragma once

#include "Graph.hpp"
#include "Types.hpp"

namespace exotic {
class ObserverEngine {
public:
    explicit ObserverEngine(Graph& graph);
    UUID Observe(const String& source, const String& data);
    GraphNode* GetObservation(UUID id);
private:
    Graph& graph_;
};
}

