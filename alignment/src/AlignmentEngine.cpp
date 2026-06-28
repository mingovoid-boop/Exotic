#include "AlignmentEngine.hpp"

namespace exotic {
AlignmentEngine::AlignmentEngine(Graph& graph) : graph_(graph) {}

AlignmentState AlignmentEngine::Evaluate(UUID nodeId) const {
    AlignmentState state;
    if (!graph_.HasNode(nodeId)) {
        state.overall = 0.0;
        state.structure = 0.0;
        state.timing = 0.0;
        state.coordination = 0.0;
        state.stability = 0.0;
        return state;
    }
    return state;
}

f64 AlignmentEngine::Score(UUID nodeId) const {
    return Evaluate(nodeId).overall;
}

}
