#pragma once
#include "exotic/cognition/agency_executive.hpp"
#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/free_agent.hpp"
#include <cstdint>
#include <optional>
#include <string>
namespace exotic::cognition {
struct IdleAgencyCycle { ThoughtCandidate candidate; AgencyCycle agency; bool persisted{false}; };
class IdleCognitionScheduler {
 public:
  explicit IdleCognitionScheduler(AgentStore& store) : store_(store) {}
  [[nodiscard]] std::optional<ThoughtCandidate> next_candidate() const;
  [[nodiscard]] bool should_run(bool externally_busy) const;
  [[nodiscard]] std::optional<IdleAgencyCycle> agency_cycle(bool externally_busy,std::int64_t now,bool persist_choice=false);
  void apply_reward(double reward);
 private: AgentStore& store_;
};
}  // namespace exotic::cognition
