#pragma once
#include "exotic/cognition/intention.hpp"
#include <optional>
#include <string>
#include <vector>
namespace exotic::cognition {
enum class ReconsiderationAction { StayCourse, Resume, Suspend, Switch, Abandon, DoNothing };
struct ReconsiderationSignal {
  double evidence_change{0.0};
  double expected_loss{0.0};
  double conflict{0.0};
  double opportunity_gain{0.0};
  double commitment_inertia{0.0};
  double staleness{0.0}; // 0=fresh, 1=strongly overdue for review; advisory only
};
struct ReconsiderationOption {
  ReconsiderationAction action{ReconsiderationAction::DoNothing};
  std::string target_intention_id;
  std::string rationale;
  double support{0.0};
};
struct ReconsiderationReview {
  bool review_worthwhile{false};
  std::string reason;
  std::vector<ReconsiderationOption> options;
};
class ReconsiderationEngine {
 public:
  [[nodiscard]] ReconsiderationReview consider(const Intention& current,const ReconsiderationSignal& signal,const std::optional<Intention>& alternative=std::nullopt) const;
};
} // namespace exotic::cognition
