#pragma once

#include "exotic/cognition/free_agent.hpp"

#include <cstdint>
#include <mutex>
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

  [[nodiscard]] std::string state_json() const;
  [[nodiscard]] std::string health_json() const;
  [[nodiscard]] std::string version_json() const;

  [[nodiscard]] ServiceThought submit_thought(std::string subject);
  bool set_mode(const std::string& mode);
  [[nodiscard]] std::string mode() const;

 private:
  static std::string now_iso8601();
  static std::string json_escape(const std::string& input);
  static std::string action_name(CognitiveAction action);

  mutable std::mutex mutex_;
  FreeAgentExecutive executive_;
  std::vector<ServiceThought> thoughts_;
  std::string mode_{"idle"};
  std::uint64_t next_thought_id_{1};
};

}  // namespace exotic::cognition
