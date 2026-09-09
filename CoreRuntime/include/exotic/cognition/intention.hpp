#pragma once
#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/volition.hpp"
#include <cstdint>
#include <string>
#include <vector>
namespace exotic::cognition {
struct ChoiceHistoryRecord {std::string id;std::string subject;std::string option_id;VolitionOutcome outcome{VolitionOutcome::DoNothing};std::string reason;std::string created_at;std::string supersedes;};
struct Intention {std::string id;std::string subject;std::string source_choice_id;std::string state{"tentative"};double strength{0.5};std::string reason;std::string created_at;std::string updated_at;};
struct IntentionTransition {std::string id;std::string intention_id;std::string from_state;std::string to_state;std::string reason;std::string created_at;};
class IntentionManager {
 public:
  explicit IntentionManager(AgentStore& store):store_(store){}
  ChoiceHistoryRecord record_choice(const std::string& subject,const VolitionDecision& decision,const std::string& created_at,std::string supersedes={});
  Intention propose_intention(const ChoiceHistoryRecord& choice,double strength,const std::string& reason);
  bool commit(const std::string& intention_id,const std::string& reason);
  bool suspend(const std::string& intention_id,const std::string& reason);
  bool abandon(const std::string& intention_id,const std::string& reason);
  bool complete(const std::string& intention_id,const std::string& reason);
  [[nodiscard]] std::vector<Intention> intentions() const;
  [[nodiscard]] std::vector<IntentionTransition> transitions(const std::string& intention_id={}) const;
  [[nodiscard]] std::vector<Intention> stale(std::int64_t now,std::int64_t stale_after_seconds) const;
  [[nodiscard]] std::vector<ChoiceHistoryRecord> choices() const;
 private: AgentStore& store_;
};
} // namespace exotic::cognition
