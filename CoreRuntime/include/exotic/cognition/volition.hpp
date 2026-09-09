#pragma once
#include <string>
#include <vector>
namespace exotic::cognition {
enum class VolitionOutcome { Choose, Defer, Reject, DoNothing };
struct VolitionOption {
  std::string id;
  std::string description;
  double desire{0.0};
  double commitment{0.0};
  double curiosity{0.0};
  double user_relevance{0.0};
  double expected_value{0.0};
  double cost{0.0};
  double risk{0.0};
};
struct VolitionDecision {
  VolitionOutcome outcome{VolitionOutcome::DoNothing};
  std::string option_id;
  std::string rationale;
  double support{0.0};
};
class VolitionEngine {
 public:
  [[nodiscard]] VolitionDecision decide(const std::vector<VolitionOption>& options) const;
 private:
  static double support(const VolitionOption& option);
};
} // namespace exotic::cognition
