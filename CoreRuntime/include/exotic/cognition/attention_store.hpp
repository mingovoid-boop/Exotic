#pragma once
#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/cognitive_budget.hpp"
#include <vector>
namespace exotic::cognition {
class AttentionStore {
 public:
  explicit AttentionStore(AgentStore& store):store_(store){}
  [[nodiscard]] std::vector<TopicAttentionState> load() const;
  void save(const TopicAttentionState& state) const;
 private: AgentStore& store_;
};
} // namespace exotic::cognition
