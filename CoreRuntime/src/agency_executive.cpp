#include "exotic/cognition/agency_executive.hpp"
namespace exotic::cognition {
AgencyCycle AgencyExecutive::consider(const AgencyInput& input) const {
  AgencyCycle c;
  c.volition = volition_.decide(input.options);
  c.metacognition = meta_.consider(input.subject, input.uncertainty, input.stakes);
  c.arbitration = arbiter_.consider(input.intention_demands, 1.0);
  c.timing = temporal_.consider(input.temporal_options, input.now);
  c.external_action_authorized = gate_.authorize(CognitiveAction::Act, input.external_action_requested && input.explicit_external_grant);
  if (input.external_action_requested && !c.external_action_authorized) {
    c.rationale = "External action remains unavailable without explicit grant. Internal choices and intentions remain inspectable and unchanged.";
  } else if (c.volition.outcome == VolitionOutcome::DoNothing) {
    c.rationale = "The executive considered available capabilities and preserved the option to do nothing.";
  } else {
    c.rationale = "The executive assembled voluntary recommendations; no recommendation automatically commits, schedules, or acts.";
  }
  return c;
}
ChoiceHistoryRecord AgencyExecutive::remember_choice(const std::string& subject,const VolitionDecision& decision,const std::string& created_at,std::string supersedes){
  return intentions_.record_choice(subject,decision,created_at,std::move(supersedes));
}
} // namespace exotic::cognition
