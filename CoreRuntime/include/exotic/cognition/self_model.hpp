#pragma once

#include "exotic/cognition/agent_store.hpp"
#include <string>
#include <vector>

namespace exotic::cognition {

struct SelfModelSnapshot {
  AgentIdentity identity;
  std::vector<PersonalityTrait> traits;
  std::vector<std::string> commitments;
  std::vector<std::string> unresolved_questions;
  std::string narrative;
};

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
