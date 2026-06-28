#pragma once

#include "../graph/include/Graph.hpp"
#include "../foundation/include/Types.hpp"

namespace exotic {
struct LearningResult {
    bool updated = false;
    f64 confidenceDelta = 0.0;
    String message;
};

class LearningEngine {
public:
    explicit LearningEngine(Graph& graph);
    LearningResult Learn(UUID nodeId, bool success);
private:
    Graph& graph_;
};
}
