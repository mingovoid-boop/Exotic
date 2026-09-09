#pragma once

#include "exotic/cognition/agent_store.hpp"

#include <cstdint>
#include <string>

namespace exotic::cognition {

class OperatorSessionManager {
 public:
  explicit OperatorSessionManager(AgentStore& store) : store_(store) {}

  [[nodiscard]] OperatorSession create(const std::string& operator_id,
                                       const std::string& raw_token,
                                       std::int64_t now_unix,
                                       std::int64_t ttl_seconds = 3600);
  [[nodiscard]] bool validate(const std::string& session_id,
                              const std::string& raw_token,
                              std::int64_t now_unix) const;
  void revoke(const std::string& session_id);

  static std::string hash_token(const std::string& raw_token);

 private:
  AgentStore& store_;
};

}  // namespace exotic::cognition
