#pragma once
#include "exotic/cognition/agent_store.hpp"
#include <string>
#include <vector>
namespace exotic::cognition {
struct ReasoningStrategy { std::string id; std::string description; double estimated_fit{0.5}; double confidence{0.5}; };
struct MetaChoice { bool engage{false}; std::string reason; std::vector<ReasoningStrategy> options; };
class MetacognitiveCapability {
 public:
  explicit MetacognitiveCapability(AgentStore& store):store_(store){}
  [[nodiscard]] MetaChoice consider(const std::string& subject,double uncertainty,double stakes) const;
  void record_outcome(const std::string& strategy,double reward,const std::string& lesson);
 private: AgentStore& store_;
};
} // namespace exotic::cognition
