#include "exotic/cognition/free_agent.hpp"

#include <algorithm>
#include <utility>

namespace exotic::cognition {

double AttentionScorer::score(const ThoughtCandidate& c) const noexcept {
  return 0.30 * c.goal_relevance +
         0.20 * c.uncertainty +
         0.15 * c.information_gain +
         0.15 * c.novelty +
         0.10 * c.risk_attention +
         0.10 * c.connection_potential -
         c.compute_cost;
}

bool ActionGate::requires_authorization(CognitiveAction action) const noexcept {
  return action == CognitiveAction::Act;
}

bool ActionGate::authorize(CognitiveAction action, bool explicit_grant) const noexcept {
  return !requires_authorization(action) || explicit_grant;
}

FreeAgentExecutive::FreeAgentExecutive(SelfState initial_state)
    : state_(std::move(initial_state)) {}

const SelfState& FreeAgentExecutive::state() const noexcept { return state_; }

const std::vector<DecisionRecord>& FreeAgentExecutive::ledger() const noexcept {
  return ledger_;
}

void FreeAgentExecutive::add_goal(Goal goal) {
  state_.goals.push_back(std::move(goal));
}

bool FreeAgentExecutive::abandon_goal(const std::string& goal_id) {
  for (auto& goal : state_.goals) {
    if (goal.id == goal_id && goal.active) {
      goal.active = false;
      return true;
    }
  }
  return false;
}

void FreeAgentExecutive::add_question(std::string question) {
  state_.unresolved_questions.push_back(std::move(question));
}

std::optional<ThoughtCandidate> FreeAgentExecutive::choose_subject(
    const std::vector<ThoughtCandidate>& candidates) const {
  if (candidates.empty()) return std::nullopt;

  const auto best = std::max_element(
      candidates.begin(), candidates.end(),
      [this](const ThoughtCandidate& a, const ThoughtCandidate& b) {
        return scorer_.score(a) < scorer_.score(b);
      });

  if (best == candidates.end() || scorer_.score(*best) <= 0.0) {
    return std::nullopt;
  }
  return *best;
}

DecisionRecord FreeAgentExecutive::cycle(
    const std::vector<ThoughtCandidate>& candidates,
    bool external_action_requested,
    bool explicit_external_grant) {
  ++state_.cognitive_cycles;

  DecisionRecord record;
  record.cycle = state_.cognitive_cycles;

  const auto chosen = choose_subject(candidates);
  if (!chosen) {
    record.action = CognitiveAction::Stop;
    record.rationale = "No candidate has positive expected cognitive value.";
    ledger_.push_back(record);
    return record;
  }

  record.candidate_id = chosen->id;
  record.subject = chosen->subject;
  record.score = scorer_.score(*chosen);

  if (external_action_requested) {
    record.external_authorization_required = true;
    if (gate_.authorize(CognitiveAction::Act, explicit_external_grant)) {
      record.action = CognitiveAction::Act;
      record.rationale = "Selected highest-value subject; external action explicitly authorized.";
    } else {
      record.action = CognitiveAction::Think;
      record.rationale = "External action withheld; continue cognition without authority escalation.";
    }
  } else {
    record.action = CognitiveAction::Think;
    record.rationale = "Selected highest-value subject for autonomous deliberation.";
  }

  ledger_.push_back(record);
  return record;
}

}  // namespace exotic::cognition
