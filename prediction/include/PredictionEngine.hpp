#pragma once

#include "Graph.hpp"
#include "Types.hpp"
#include <vector>

namespace exotic {
struct Prediction {
    UUID target;
    f64 confidence = 0.0;
    String description;
};

class PredictionEngine {
public:
    explicit PredictionEngine(Graph& graph);
    Prediction Predict(UUID nodeId) const;
    std::vector<Prediction> PredictNeighbors(UUID nodeId) const;
private:
    Graph& graph_;
};
}

