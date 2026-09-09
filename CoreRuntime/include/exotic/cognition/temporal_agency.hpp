#pragma once
#include "exotic/cognition/intention.hpp"
#include <cstdint>
#include <string>
#include <vector>
namespace exotic::cognition {
enum class TemporalChoice { Now, Later, Resume, Wait, Skip };
struct TemporalOption { Intention intention; std::int64_t earliest{0}; std::int64_t deadline{0}; std::int64_t last_active{0}; std::int64_t recurrence_seconds{0}; double urgency{0.0}; double delayed_value{0.5}; double interruption_cost{0.0}; bool interrupted{false}; };
struct TemporalDecision { std::string intention_id; TemporalChoice choice{TemporalChoice::Wait}; std::int64_t suggested_time{0}; std::string rationale; };
class TemporalAgency {
 public:
  [[nodiscard]] std::vector<TemporalDecision> consider(const std::vector<TemporalOption>& options,std::int64_t now) const;
};
} // namespace exotic::cognition
