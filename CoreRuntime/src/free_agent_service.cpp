#include "exotic/cognition/free_agent_service.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace exotic::cognition {

FreeAgentService::FreeAgentService() {
  executive_.add_goal({"goal-1", "Preserve coherent autonomous cognition", 1.0, true, true});
  executive_.add_goal({"goal-2", "Investigate high-value unresolved questions", 0.85, true, true});
}

std::string FreeAgentService::now_iso8601() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm tm{};
#ifdef _WIN32
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif
  std::ostringstream out;
  out << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
  return out.str();
}

std::string FreeAgentService::json_escape(const std::string& input) {
  std::ostringstream out;
  for (const char ch : input) {
    switch (ch) {
      case '\\': out << "\\\\"; break;
      case '"': out << "\\\""; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default: out << ch; break;
    }
  }
  return out.str();
}

std::string FreeAgentService::action_name(CognitiveAction action) {
  switch (action) {
    case CognitiveAction::Think: return "thinking";
    case CognitiveAction::Act: return "acting";
    case CognitiveAction::Wait: return "waiting";
    case CognitiveAction::Stop: return "stopped";
    default: return "idle";
  }
}

ServiceThought FreeAgentService::submit_thought(std::string subject) {
  if (subject.empty()) throw std::invalid_argument("subject must not be empty");

  std::lock_guard lock(mutex_);
  const std::string id = "thought-" + std::to_string(next_thought_id_++);
  ThoughtCandidate candidate{
      id,
      subject,
      0.70,
      0.55,
      0.65,
      0.50,
      0.10,
      0.45,
      0.05,
  };

  const auto record = executive_.cycle({candidate}, false, false);
  mode_ = action_name(record.action);

  ServiceThought thought;
  thought.id = id;
  thought.subject = std::move(subject);
  thought.score = record.score;
  thought.reason = record.rationale;
  thought.status = record.action == CognitiveAction::Think ? "selected" : "queued";
  thought.created_at = now_iso8601();
  thoughts_.insert(thoughts_.begin(), thought);
  return thought;
}

bool FreeAgentService::set_mode(const std::string& mode) {
  if (mode != "idle" && mode != "thinking" && mode != "waiting" &&
      mode != "acting" && mode != "stopped") {
    return false;
  }
  std::lock_guard lock(mutex_);
  mode_ = mode;
  return true;
}

std::string FreeAgentService::mode() const {
  std::lock_guard lock(mutex_);
  return mode_;
}

std::string FreeAgentService::health_json() const {
  return "{\"status\":\"ok\",\"service\":\"exotic-free-agent\"}";
}

std::string FreeAgentService::version_json() const {
  return "{\"name\":\"EXOTIC Free-Agent Runtime\",\"version\":\"0.2.0\",\"api\":\"v1\"}";
}

std::string FreeAgentService::state_json() const {
  std::lock_guard lock(mutex_);
  const auto& state = executive_.state();
  const auto& ledger = executive_.ledger();

  std::string active_goal;
  double top_priority = -1.0;
  for (const auto& goal : state.goals) {
    if (goal.active && goal.priority > top_priority) {
      top_priority = goal.priority;
      active_goal = goal.description;
    }
  }

  std::string active_thought;
  if (!ledger.empty()) active_thought = ledger.back().subject;

  std::ostringstream out;
  out << "{\"agent\":{";
  out << "\"id\":\"free-agent-01\",";
  out << "\"mode\":\"" << json_escape(mode_) << "\",";
  if (!active_goal.empty()) out << "\"activeGoal\":\"" << json_escape(active_goal) << "\",";
  if (!active_thought.empty()) out << "\"activeThought\":\"" << json_escape(active_thought) << "\",";
  out << "\"autonomy\":1.0,\"externalAuthority\":0.0,";
  out << "\"updatedAt\":\"" << now_iso8601() << "\"},";

  out << "\"thoughts\":[";
  for (std::size_t i = 0; i < thoughts_.size(); ++i) {
    if (i) out << ',';
    const auto& t = thoughts_[i];
    out << "{\"id\":\"" << json_escape(t.id) << "\","
        << "\"subject\":\"" << json_escape(t.subject) << "\","
        << "\"score\":" << t.score << ','
        << "\"reason\":\"" << json_escape(t.reason) << "\","
        << "\"status\":\"" << json_escape(t.status) << "\","
        << "\"createdAt\":\"" << json_escape(t.created_at) << "\"}";
  }
  out << "],\"goals\":[";
  for (std::size_t i = 0; i < state.goals.size(); ++i) {
    if (i) out << ',';
    const auto& g = state.goals[i];
    out << "{\"id\":\"" << json_escape(g.id) << "\","
        << "\"title\":\"" << json_escape(g.description) << "\","
        << "\"priority\":" << g.priority << ','
        << "\"source\":\"" << (g.self_generated ? "self" : "system") << "\","
        << "\"status\":\"" << (g.active ? "active" : "abandoned") << "\"}";
  }
  out << "]}";
  return out.str();
}

}  // namespace exotic::cognition
