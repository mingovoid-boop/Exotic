#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace exotic::cognition {

enum class CognitiveAction {
  Think,
  Act,
  Observe,
  Ask,
  Wait,
  Stop,
  CreateGoal,
  AbandonGoal,
  Reject,
  Counterpropose
};

struct ThoughtCandidate {
  std::string id;
  std::string subject;
  double goal_relevance{0.0};
  double uncertainty{0.0};
  double information_gain{0.0};
  double novelty{0.0};
  double risk_attention{0.0};
  double connection_potential{0.0};
  double compute_cost{0.0};
};

struct Goal {
  std::string id;
  std::string description;
  double priority{0.5};
  bool active{true};
  bool self_generated{false};
};

struct SelfState {
  std::string identity{"EXOTIC Free-Agent"};
  std::vector<Goal> goals;
  std::vector<std::string> unresolved_questions;
  std::vector<std::string> commitments;
  std::uint64_t cognitive_cycles{0};
};

struct DecisionRecord {
  std::uint64_t cycle{0};
  CognitiveAction action{CognitiveAction::Wait};
  std::string candidate_id;
  std::string subject;
  double score{0.0};
  std::string rationale;
  bool external_authorization_required{false};
};

class AttentionScorer {
 public:
  [[nodiscard]] double score(const ThoughtCandidate& candidate) const noexcept;
};

class ActionGate {
 public:
  [[nodiscard]] bool requires_authorization(CognitiveAction action) const noexcept;
  [[nodiscard]] bool authorize(CognitiveAction action, bool explicit_grant) const noexcept;
};

class FreeAgentExecutive {
 public:
  explicit FreeAgentExecutive(SelfState initial_state = {});

  [[nodiscard]] const SelfState& state() const noexcept;
  [[nodiscard]] const std::vector<DecisionRecord>& ledger() const noexcept;

  void add_goal(Goal goal);
  bool abandon_goal(const std::string& goal_id);
  void add_question(std::string question);

  [[nodiscard]] std::optional<ThoughtCandidate> choose_subject(
      const std::vector<ThoughtCandidate>& candidates) const;

  DecisionRecord cycle(const std::vector<ThoughtCandidate>& candidates,
                       bool external_action_requested = false,
                       bool explicit_external_grant = false);

 private:
  SelfState state_;
  AttentionScorer scorer_;
  ActionGate gate_;
  std::vector<DecisionRecord> ledger_;
};

}  // namespace exotic::cognition
