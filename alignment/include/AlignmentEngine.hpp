#pragma once

#include "Graph.hpp"
#include "Types.hpp"

namespace exotic {
struct AlignmentState {
    f64 overall = 1.0;
    f64 structure = 1.0;
    f64 timing = 1.0;
    f64 coordination = 1.0;
    f64 stability = 1.0;
};

class AlignmentEngine {
public:
    explicit AlignmentEngine(Graph& graph);
    AlignmentState Evaluate(UUID nodeId) const;
    f64 Score(UUID nodeId) const;
private:
    Graph& graph_;
};
}

