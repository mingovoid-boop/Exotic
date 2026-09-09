#pragma once
#include "exotic/cognition/agent_store.hpp"
#include <string>
#include <vector>
namespace exotic::cognition {
struct BeliefEvidence { std::string id; std::string belief_id; std::string source; std::string statement; double direction{0.0}; double reliability{0.5}; std::string observed_at; };
struct BeliefRevisionProposal { std::string belief_id; std::string old_proposition; std::string proposed_proposition; double old_confidence{0.5}; double proposed_confidence{0.5}; std::vector<BeliefEvidence> evidence; std::string rationale; };
struct BeliefRevisionRecord { std::string id; std::string belief_id; std::string before_proposition; std::string after_proposition; double before_confidence{0.5}; double after_confidence{0.5}; std::string rationale; std::string created_at; };
class BeliefRevisionCapability {
 public:
  explicit BeliefRevisionCapability(AgentStore& store):store_(store){}
  [[nodiscard]] BeliefRevisionProposal consider(const std::string& belief_id,const std::vector<BeliefEvidence>& evidence,std::string alternative={}) const;
  [[nodiscard]] bool apply(const BeliefRevisionProposal& proposal,const std::string& created_at);
  [[nodiscard]] std::vector<BeliefRevisionRecord> history(const std::string& belief_id={}) const;
 private: AgentStore& store_;
};
} // namespace exotic::cognition
