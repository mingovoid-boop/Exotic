#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct sqlite3;

namespace exotic::cognition {

struct AgentIdentity { std::string id{"free-agent-01"}; std::string name{"EXOTIC Free-Agent"}; std::string self_description{"A persistent autonomous cognitive agent."}; std::string avatar_kind{"geometric"}; std::string avatar_value{"hex-eye"}; };
struct BeliefRecord { std::string id; std::string proposition; double confidence{0.5}; std::string evidence; std::string updated_at; };
struct PersistentGoal { std::string id; std::string title; double priority{0.5}; std::string source{"self"}; std::string status{"active"}; };
struct PreferenceRecord { std::string key; double weight{0.5}; };
struct DriveRecord { std::string key; double level{0.5}; double target{0.5}; double learning_rate{0.05}; };
struct RewardEvent { std::string id; std::string source; double value{0.0}; std::string reason; std::string created_at; };
struct OperatorSession { std::string id; std::string operator_id; std::string token_hash; std::int64_t expires_unix{0}; bool revoked{false}; };
struct Episode { std::string id; std::string summary; double salience{0.5}; double reward{0.0}; std::string created_at; };
struct SemanticMemory { std::string id; std::string concept; std::string statement; double confidence{0.5}; std::string source_episode; };
struct PersonalityTrait { std::string key; double value{0.5}; double plasticity{0.05}; };
struct Reflection { std::string id; std::string observation; std::string lesson; double confidence{0.5}; std::string created_at; };

class AgentStore {
 public:
  explicit AgentStore(std::string path); ~AgentStore(); AgentStore(const AgentStore&) = delete; AgentStore& operator=(const AgentStore&) = delete;
  void initialize(); [[nodiscard]] const std::string& path() const noexcept;
  void save_identity(const AgentIdentity&); [[nodiscard]] AgentIdentity load_identity() const;
  void upsert_belief(const BeliefRecord&); [[nodiscard]] std::vector<BeliefRecord> beliefs() const;
  void upsert_goal(const PersistentGoal&); [[nodiscard]] std::vector<PersistentGoal> goals() const;
  void set_preference(const PreferenceRecord&); [[nodiscard]] std::vector<PreferenceRecord> preferences() const;
  void set_drive(const DriveRecord&); [[nodiscard]] std::vector<DriveRecord> drives() const;
  void append_reward(const RewardEvent&); [[nodiscard]] double reward_total() const;
  void create_session(const OperatorSession&); [[nodiscard]] std::optional<OperatorSession> find_session(const std::string&) const; void revoke_session(const std::string&);
  void append_thought(const std::string&,const std::string&,double,const std::string&,const std::string&,const std::string&);
  void append_episode(const Episode&); [[nodiscard]] std::vector<Episode> episodes(std::size_t limit=32) const;
  void upsert_semantic_memory(const SemanticMemory&); [[nodiscard]] std::vector<SemanticMemory> semantic_memories() const;
  void set_trait(const PersonalityTrait&); [[nodiscard]] std::vector<PersonalityTrait> traits() const;
  void append_reflection(const Reflection&); [[nodiscard]] std::vector<Reflection> reflections(std::size_t limit=32) const;
 private: void exec(const std::string&) const; sqlite3* db_{nullptr}; std::string path_;
};

}  // namespace exotic::cognition
