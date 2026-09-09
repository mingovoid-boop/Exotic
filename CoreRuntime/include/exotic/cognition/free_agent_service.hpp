#pragma once

#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/free_agent.hpp"
#include "exotic/cognition/idle_cognition.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace exotic::cognition {

struct ServiceThought {
  std::string id;
  std::string subject;
  double score{0.0};
  std::string reason;
  std::string status{"queued"};
  std::string created_at;
};

class FreeAgentService {
 public:
  FreeAgentService();
  explicit FreeAgentService(std::string journal_path);
  FreeAgentService(std::string journal_path, std::string database_path);

  [[nodiscard]] std::string state_json() const;
  [[nodiscard]] std::string health_json() const;
  [[nodiscard]] std::string version_json() const;

  [[nodiscard]] ServiceThought submit_thought(std::string subject);
  [[nodiscard]] std::optional<ServiceThought> idle_tick(bool externally_busy = false);
  void set_identity(const AgentIdentity& identity);
  void add_belief(const BeliefRecord& belief);
  void upsert_persistent_goal(const PersistentGoal& goal);
  void set_preference(const PreferenceRecord& preference);
  void set_drive(const DriveRecord& drive);
  void reward(double value, std::string source, std::string reason);

  bool set_mode(const std::string& mode);
  [[nodiscard]] std::string mode() const;
  [[nodiscard]] AgentStore* store() noexcept { return store_.get(); }
  [[nodiscard]] const AgentStore* store() const noexcept { return store_.get(); }

 private:
  static std::string now_iso8601();
  static std::string json_escape(const std::string& input);
  static std::string action_name(CognitiveAction action);
  static std::string field_encode(const std::string& input);
  static std::string field_decode(const std::string& input);

  void initialize_goals();
  void initialize_persistent_state();
  void load_journal();
  void append_journal(const ServiceThought& thought) const;
  void persist_thought(const ServiceThought& thought);

  mutable std::mutex mutex_;
  FreeAgentExecutive executive_;
  std::vector<ServiceThought> thoughts_;
  std::string mode_{"idle"};
  std::string journal_path_;
  std::unique_ptr<AgentStore> store_;
  std::unique_ptr<IdleCognitionScheduler> idle_scheduler_;
  std::uint64_t next_thought_id_{1};
  std::uint64_t next_reward_id_{1};
};

}  // namespace exotic::cognition
