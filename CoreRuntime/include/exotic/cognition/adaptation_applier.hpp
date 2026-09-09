#pragma once
#include "exotic/cognition/adaptation.hpp"
#include "exotic/cognition/agent_store.hpp"
#include <string>
#include <vector>
namespace exotic::cognition {
struct AdaptationHistoryRecord {
  std::string id;
  AdaptationTarget target{AdaptationTarget::Belief};
  std::string target_id;
  double before_value{0.0};
  double after_value{0.0};
  std::string rationale;
  std::string created_at;
  bool rolled_back{false};
};
class AdaptationApplier {
 public:
  explicit AdaptationApplier(AgentStore& store):store_(store){}
  [[nodiscard]] bool apply(const AdaptationChoice& choice,const std::string& created_at,std::string rationale={});
  [[nodiscard]] bool rollback(const std::string& history_id,const std::string& reason);
  [[nodiscard]] std::vector<AdaptationHistoryRecord> history() const;
 private:
  AgentStore& store_;
};
} // namespace exotic::cognition
