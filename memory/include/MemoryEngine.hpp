#pragma once

#include "Graph.hpp"
#include "Types.hpp"
#include <vector>

namespace exotic {
class MemoryEngine {
public:
    explicit MemoryEngine(Graph& graph);
    UUID Store(const String& name, const String& value);
    GraphNode* Recall(UUID id);
    std::vector<GraphNode*> Search(const String& text);
private:
    Graph& graph_;
};
}

