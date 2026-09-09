#pragma once
#include "exotic/cognition/intention.hpp"
#include <string>
#include <vector>
namespace exotic::cognition {
enum class ArbitrationDisposition { Run, Yield, Cooperate, Blocked, Unscheduled };
struct IntentionDemand { Intention intention; double urgency{0.5}; double expected_value{0.5}; double attention_cost{0.0}; double resource_cost{0.0}; std::vector<std::string> cooperates_with; std::vector<std::string> conflicts_with; };
struct ArbitrationAssignment { std::string intention_id; ArbitrationDisposition disposition{ArbitrationDisposition::Unscheduled}; double allocation{0.0}; std::string rationale; };
struct ArbitrationPlan { std::vector<ArbitrationAssignment> assignments; double unused_capacity{1.0}; };
class MultiIntentionArbiter {
 public:
  [[nodiscard]] ArbitrationPlan consider(const std::vector<IntentionDemand>& demands,double capacity=1.0) const;
};
} // namespace exotic::cognition
