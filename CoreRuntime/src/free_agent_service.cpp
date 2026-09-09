#include "exotic/cognition/free_agent_service.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace exotic::cognition {

FreeAgentService::FreeAgentService() {
  initialize_goals();
}

FreeAgentService::FreeAgentService(std::string journal_path)
    : journal_path_(std::move(journal_path)) {
  initialize_goals();
  load_journal();
}

void FreeAgentService::initialize_goals() {
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

std::string FreeAgentService::field_encode(const std::string& input) {
  static constexpr char hex[] = "0123456789ABCDEF";
  std::string out;
  out.reserve(input.size());
  for (const unsigned char ch : input) {
    if (ch == '%' || ch == '\t' || ch == '\n' || ch == '\r') {
      out.push_back('%');
      out.push_back(hex[(ch >> 4) & 0x0F]);
      out.push_back(hex[ch & 0x0F]);
    } else {
      out.push_back(static_cast<char>(ch));
    }
  }
  return out;
}

std::string FreeAgentService::field_decode(const std::string& input) {
  auto value = [](char ch) -> int {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return -1;
  };

  std::string out;
  out.reserve(input.size());
  for (std::size_t i = 0; i < input.size(); ++i) {
    if (input[i] == '%' && i + 2 < input.size()) {
      const int hi = value(input[i + 1]);
      const int lo = value(input[i + 2]);
      if (hi >= 0 && lo >= 0) {
        out.push_back(static_cast<char>((hi << 4) | lo));
        i += 2;
        continue;
      }
    }
    out.push_back(input[i]);
  }
  return out;
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

void FreeAgentService::load_journal() {
  if (journal_path_.empty()) return;
  std::ifstream in(journal_path_);
  if (!in) return;

  std::string line;
  while (std::getline(in, line)) {
    std::istringstream row(line);
    std::string id, created, status, score_text, subject, reason;
    if (!std::getline(row, id, '\t') ||
        !std::getline(row, created, '\t') ||
        !std::getline(row, status, '\t') ||
        !std::getline(row, score_text, '\t') ||
        !std::getline(row, subject, '\t') ||
        !std::getline(row, reason)) {
      continue;
    }

    ServiceThought thought;
    thought.id = field_decode(id);
    thought.created_at = field_decode(created);
    thought.status = field_decode(status);
    thought.subject = field_decode(subject);
    thought.reason = field_decode(reason);
    try {
      thought.score = std::stod(score_text);
    } catch (...) {
      thought.score = 0.0;
    }

    thoughts_.insert(thoughts_.begin(), thought);

    constexpr const char* prefix = "thought-";
    if (thought.id.rfind(prefix, 0) == 0) {
      try {
        const auto number = static_cast<std::uint64_t>(std::stoull(thought.id.substr(8)));
        if (number >= next_thought_id_) next_thought_id_ = number + 1;
      } catch (...) {
      }
    }
  }
}

void FreeAgentService::append_journal(const ServiceThought& thought) const {
  if (journal_path_.empty()) return;

  const std::filesystem::path path(journal_path_);
  if (path.has_parent_path()) {
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) throw std::runtime_error("unable to create journal directory");
  }

  std::ofstream out(journal_path_, std::ios::app);
  if (!out) throw std::runtime_error("unable to open cognition journal");
  out << field_encode(thought.id) << '\t'
      << field_encode(thought.created_at) << '\t'
      << field_encode(thought.status) << '\t'
      << thought.score << '\t'
      << field_encode(thought.subject) << '\t'
      << field_encode(thought.reason) << '\n';
  out.flush();
  if (!out) throw std::runtime_error("unable to persist cognition journal");
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
  append_journal(thought);
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
  if (!ledger.empty()) {
    active_thought = ledger.back().subject;
  } else if (!thoughts_.empty()) {
    active_thought = thoughts_.front().subject;
  }

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
