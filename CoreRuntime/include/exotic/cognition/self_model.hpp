#pragma once

#include "exotic/cognition/agent_store.hpp"
#include <string>
#include <vector>

namespace exotic::cognition {

struct Episode {
  std::string id;
  std::string summary;
  double salience{0.5};
  double reward{0.0};
  std::string created_at;
};

struct SemanticMemory {
  std::string id;
  std::string concept;
  std::string statement;
  double confidence{0.5};
  std::string source_episode;
};

struct PersonalityTrait {
  std::string key;
  double value{0.5};
  double plasticity{0.05};
};

struct Reflection {
  std::string id;
  std::string observation;
  std::string lesson;
  double confidence{0.5};
  std::string created_at;
};

struct SelfModelSnapshot {
  AgentIdentity identity;
  std::vector<PersonalityTrait> traits;
  std::vector<std::string> commitments;
  std::vector<std::string> unresolved_questions;
  std::string narrative;
};

// Development is deliberately bounded: experience may update ordinary
// preferences, traits and candidate goals, but never permissions, ActionGate,
// operator policy, or constitutional constraints.
class DevelopmentalSelfModel {
 public:
  explicit DevelopmentalSelfModel(AgentStore& store) : store_(store) {}

  void remember(const Episode& episode);
  void consolidate();
  void reflect();
  void adapt_preferences(double max_delta = 0.05);
  void generate_candidate_goals();
  [[nodiscard]] SelfModelSnapshot snapshot() const;

 private:
  AgentStore& store_;
};

}  // namespace exotic::cognition
