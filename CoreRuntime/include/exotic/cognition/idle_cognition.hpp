#pragma once

#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/free_agent.hpp"

#include <optional>
#include <string>

namespace exotic::cognition {

class IdleCognitionScheduler {
 public:
  explicit IdleCognitionScheduler(AgentStore& store) : store_(store) {}

  [[nodiscard]] std::optional<ThoughtCandidate> next_candidate() const;
  [[nodiscard]] bool should_run(bool externally_busy) const;
  void apply_reward(double reward);

 private:
  AgentStore& store_;
};

}  // namespace exotic::cognition
