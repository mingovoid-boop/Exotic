#include "exotic/cognition/free_agent.hpp"

#include <cassert>
#include <vector>

using exotic::cognition::CognitiveAction;
using exotic::cognition::FreeAgentExecutive;
using exotic::cognition::Goal;
using exotic::cognition::ThoughtCandidate;

int main() {
  FreeAgentExecutive agent;
  agent.add_goal(Goal{"g1", "Improve runtime reliability", 0.9, true, false});
  agent.add_question("Why do repeated failures share the same pattern?");

  std::vector<ThoughtCandidate> candidates{
      {"distraction", "Explore an unrelated novelty", 0.05, 0.2, 0.1, 0.95, 0.0, 0.1, 0.25},
      {"failure-pattern", "Investigate repeated runtime failures", 0.95, 0.8, 0.9, 0.5, 0.7, 0.8, 0.10},
      {"minor-cleanup", "Rename a cosmetic label", 0.2, 0.1, 0.1, 0.1, 0.0, 0.1, 0.05},
  };

  const auto chosen = agent.choose_subject(candidates);
  assert(chosen.has_value());
  assert(chosen->id == "failure-pattern");

  const auto thought = agent.cycle(candidates);
  assert(thought.action == CognitiveAction::Think);
  assert(thought.candidate_id == "failure-pattern");
  assert(!thought.external_authorization_required);

  const auto blocked = agent.cycle(candidates, true, false);
  assert(blocked.action == CognitiveAction::Think);
  assert(blocked.external_authorization_required);

  const auto authorized = agent.cycle(candidates, true, true);
  assert(authorized.action == CognitiveAction::Act);
  assert(authorized.external_authorization_required);

  std::vector<ThoughtCandidate> worthless{
      {"noise", "Low-value noise", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.2},
  };
  const auto stopped = agent.cycle(worthless);
  assert(stopped.action == CognitiveAction::Stop);

  assert(agent.ledger().size() == 4);
  assert(agent.state().cognitive_cycles == 4);
  assert(agent.abandon_goal("g1"));
  assert(!agent.state().goals.front().active);

  return 0;
}
