#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
namespace exotic::cognition {
enum class AttentionDisposition { Consider, Cooldown, BudgetExhausted, StopTopic };
struct TopicAttentionState { std::string topic_id; std::int64_t last_considered{0}; std::int64_t cooldown_until{0}; std::uint32_t repeats_in_window{0}; bool stopped{false}; };
struct AttentionDecision { AttentionDisposition disposition{AttentionDisposition::Consider}; double novelty_multiplier{1.0}; double remaining_budget{1.0}; std::string rationale; };
class CognitiveBudget {
 public:
  explicit CognitiveBudget(double capacity=1.0):capacity_(capacity),remaining_(capacity){}
  void reset(double capacity=-1.0);
  void stop_topic(const std::string& topic_id);
  void resume_topic(const std::string& topic_id);
  [[nodiscard]] AttentionDecision consider(const std::string& topic_id,std::int64_t now,double cost,double novelty);
  [[nodiscard]] double remaining() const noexcept{return remaining_;}
 private:
  double capacity_{1.0};
  double remaining_{1.0};
  std::unordered_map<std::string,TopicAttentionState> topics_;
};
} // namespace exotic::cognition
