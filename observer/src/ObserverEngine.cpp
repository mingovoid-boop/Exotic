#include "ObserverEngine.hpp"

namespace exotic {
ObserverEngine::ObserverEngine(Graph& graph) : graph_(graph) {}
UUID ObserverEngine::Observe(const String& source, const String& data) { GraphNode node; node.name = source; node.metadata["type"] = "observation"; node.metadata["source"] = source; node.metadata["data"] = data; return graph_.AddNode(node); }
GraphNode* ObserverEngine::GetObservation(UUID id) { return graph_.FindNode(id); }
}
