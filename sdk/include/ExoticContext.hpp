#pragma once

#include "Exotic.hpp"

namespace exotic {

class ExoticContext {
public:
    ExoticContext();

    Graph graph;
    Config config;
    SDKEventBus events;
    EngineRegistry registry;

    MemoryEngine memory;
    ObserverEngine observer;
    AlignmentEngine alignment;
    PredictionEngine prediction;
    ExecutionEngine execution;
    LearningEngine learning;
    RelationshipEngine relationship;
};

inline ExoticContext::ExoticContext()
    : memory(graph),
      observer(graph),
      alignment(graph),
      prediction(graph),
      execution(graph),
      learning(graph),
      relationship(graph)
{
}

} // namespace exotic