#include "MemoryEngine.hpp"

namespace exotic {

MemoryEngine::MemoryEngine(Graph& graph)
    : graph_(graph) {}

UUID MemoryEngine::Store(const String& name, const String& value)
{
    GraphNode node;
    node.name = name;
    node.metadata["type"] = "memory";
    node.metadata["value"] = value;
    return graph_.AddNode(node);
}

GraphNode* MemoryEngine::Recall(UUID id)
{
    return graph_.FindNode(id);
}

std::vector<GraphNode*> MemoryEngine::Search(const String& text)
{
    std::vector<GraphNode*> results;

    for (auto& pair : const_cast<std::unordered_map<UUID, GraphNode>&>(graph_.Nodes()))
    {
        auto valueIt = pair.second.metadata.find("value");

        bool nameMatch = pair.second.name.find(text) != String::npos;
        bool valueMatch =
            valueIt != pair.second.metadata.end() &&
            valueIt->second.find(text) != String::npos;

        if (nameMatch || valueMatch)
            results.push_back(&pair.second);
    }

    return results;
}

} // namespace exotic
