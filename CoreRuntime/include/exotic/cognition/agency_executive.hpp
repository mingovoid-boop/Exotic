#pragma once
#include "exotic/cognition/arbitration.hpp"
#include "exotic/cognition/free_agent.hpp"
#include "exotic/cognition/intention.hpp"
#include "exotic/cognition/metacognition.hpp"
#include "exotic/cognition/reconsideration.hpp"
#include "exotic/cognition/temporal_agency.hpp"
#include "exotic/cognition/volition.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace exotic::cognition {
struct AgencyInput {
  std::string subject;
  std::vector<VolitionOption> options;
  std::vector<IntentionDemand> intention_demands;
  std::vector<TemporalOption> temporal_options;
  double uncertainty{0.0};
  double stakes{0.0};
  std::int64_t now{0};
  bool external_action_requested{false};
  bool explicit_external_grant{false};
};
struct AgencyCycle {
  VolitionDecision volition;
  MetaChoice metacognition;
  ArbitrationPlan arbitration;
  std::vector<TemporalDecision> timing;
  bool external_action_authorized{false};
  std::string rationale;
};
class AgencyExecutive {
 public:
  explicit AgencyExecutive(AgentStore& store):store_(store),meta_(store),intentions_(store){}
  [[nodiscard]] AgencyCycle consider(const AgencyInput& input) const;
  ChoiceHistoryRecord remember_choice(const std::string& subject,const VolitionDecision& decision,const std::string& created_at,std::string supersedes={});
 private:
  AgentStore& store_;
  MetacognitiveCapability meta_;
  mutable IntentionManager intentions_;
  VolitionEngine volition_;
  MultiIntentionArbiter arbiter_;
  TemporalAgency temporal_;
  ActionGate gate_;
};
} // namespace exotic::cognition
