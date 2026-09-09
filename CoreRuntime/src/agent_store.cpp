#include "exotic/cognition/agent_store.hpp"

#include <sqlite3.h>
#include <stdexcept>
#include <utility>

namespace exotic::cognition {
namespace {

void check(int rc, sqlite3* db, const char* what) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    throw std::runtime_error(std::string(what) + ": " + sqlite3_errmsg(db));
  }
}

class Statement {
 public:
  Statement(sqlite3* db, const char* sql) : db_(db) {
    check(sqlite3_prepare_v2(db, sql, -1, &stmt_, nullptr), db, "prepare");
  }
  ~Statement() { sqlite3_finalize(stmt_); }
  sqlite3_stmt* get() const { return stmt_; }
 private:
  sqlite3* db_{};
  sqlite3_stmt* stmt_{};
};

void bind_text(sqlite3_stmt* stmt, int index, const std::string& value) {
  sqlite3_bind_text(stmt, index, value.c_str(), static_cast<int>(value.size()), SQLITE_TRANSIENT);
}

std::string col_text(sqlite3_stmt* stmt, int index) {
  const auto* value = sqlite3_column_text(stmt, index);
  return value ? reinterpret_cast<const char*>(value) : "";
}

}  // namespace

AgentStore::AgentStore(std::string path) : path_(std::move(path)) {
  check(sqlite3_open(path_.c_str(), &db_), db_, "open database");
  exec("PRAGMA journal_mode=WAL;");
  exec("PRAGMA foreign_keys=ON;");
  exec("PRAGMA synchronous=NORMAL;");
}

AgentStore::~AgentStore() {
  if (db_) sqlite3_close(db_);
}

const std::string& AgentStore::path() const noexcept { return path_; }

void AgentStore::exec(const std::string& sql) const {
  char* error = nullptr;
  const int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error);
  if (rc != SQLITE_OK) {
    const std::string message = error ? error : sqlite3_errmsg(db_);
    sqlite3_free(error);
    throw std::runtime_error(message);
  }
}

void AgentStore::initialize() {
  exec(R"SQL(
CREATE TABLE IF NOT EXISTS identity(
  singleton INTEGER PRIMARY KEY CHECK(singleton=1),
  id TEXT NOT NULL, name TEXT NOT NULL, self_description TEXT NOT NULL,
  avatar_kind TEXT NOT NULL, avatar_value TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS beliefs(
  id TEXT PRIMARY KEY, proposition TEXT NOT NULL, confidence REAL NOT NULL CHECK(confidence BETWEEN 0 AND 1),
  evidence TEXT NOT NULL DEFAULT '', updated_at TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS goals(
  id TEXT PRIMARY KEY, title TEXT NOT NULL, priority REAL NOT NULL CHECK(priority BETWEEN 0 AND 1),
  source TEXT NOT NULL, status TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS preferences(
  key TEXT PRIMARY KEY, weight REAL NOT NULL CHECK(weight BETWEEN -1 AND 1)
);
CREATE TABLE IF NOT EXISTS drives(
  key TEXT PRIMARY KEY, level REAL NOT NULL, target REAL NOT NULL, learning_rate REAL NOT NULL
);
CREATE TABLE IF NOT EXISTS rewards(
  id TEXT PRIMARY KEY, source TEXT NOT NULL, value REAL NOT NULL, reason TEXT NOT NULL, created_at TEXT NOT NULL
);
CREATE TABLE IF NOT EXISTS operator_sessions(
  id TEXT PRIMARY KEY, operator_id TEXT NOT NULL, token_hash TEXT NOT NULL,
  expires_unix INTEGER NOT NULL, revoked INTEGER NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS thoughts(
  id TEXT PRIMARY KEY, subject TEXT NOT NULL, score REAL NOT NULL,
  reason TEXT NOT NULL, status TEXT NOT NULL, created_at TEXT NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_goals_status_priority ON goals(status, priority DESC);
CREATE INDEX IF NOT EXISTS idx_beliefs_confidence ON beliefs(confidence DESC);
CREATE INDEX IF NOT EXISTS idx_rewards_created ON rewards(created_at DESC);
)SQL");
}

void AgentStore::save_identity(const AgentIdentity& v) {
  Statement s(db_, "INSERT INTO identity(singleton,id,name,self_description,avatar_kind,avatar_value) VALUES(1,?,?,?,?,?) ON CONFLICT(singleton) DO UPDATE SET id=excluded.id,name=excluded.name,self_description=excluded.self_description,avatar_kind=excluded.avatar_kind,avatar_value=excluded.avatar_value;");
  bind_text(s.get(), 1, v.id); bind_text(s.get(), 2, v.name); bind_text(s.get(), 3, v.self_description);
  bind_text(s.get(), 4, v.avatar_kind); bind_text(s.get(), 5, v.avatar_value);
  check(sqlite3_step(s.get()), db_, "save identity");
}

AgentIdentity AgentStore::load_identity() const {
  Statement s(db_, "SELECT id,name,self_description,avatar_kind,avatar_value FROM identity WHERE singleton=1;");
  AgentIdentity v;
  if (sqlite3_step(s.get()) == SQLITE_ROW) {
    v.id = col_text(s.get(),0); v.name = col_text(s.get(),1); v.self_description = col_text(s.get(),2);
    v.avatar_kind = col_text(s.get(),3); v.avatar_value = col_text(s.get(),4);
  }
  return v;
}

void AgentStore::upsert_belief(const BeliefRecord& v) {
  Statement s(db_, "INSERT INTO beliefs(id,proposition,confidence,evidence,updated_at) VALUES(?,?,?,?,?) ON CONFLICT(id) DO UPDATE SET proposition=excluded.proposition,confidence=excluded.confidence,evidence=excluded.evidence,updated_at=excluded.updated_at;");
  bind_text(s.get(),1,v.id); bind_text(s.get(),2,v.proposition); sqlite3_bind_double(s.get(),3,v.confidence); bind_text(s.get(),4,v.evidence); bind_text(s.get(),5,v.updated_at);
  check(sqlite3_step(s.get()), db_, "upsert belief");
}

std::vector<BeliefRecord> AgentStore::beliefs() const {
  Statement s(db_, "SELECT id,proposition,confidence,evidence,updated_at FROM beliefs ORDER BY confidence DESC,id;");
  std::vector<BeliefRecord> out;
  while (sqlite3_step(s.get()) == SQLITE_ROW) out.push_back({col_text(s.get(),0),col_text(s.get(),1),sqlite3_column_double(s.get(),2),col_text(s.get(),3),col_text(s.get(),4)});
  return out;
}

void AgentStore::upsert_goal(const PersistentGoal& v) {
  Statement s(db_, "INSERT INTO goals(id,title,priority,source,status) VALUES(?,?,?,?,?) ON CONFLICT(id) DO UPDATE SET title=excluded.title,priority=excluded.priority,source=excluded.source,status=excluded.status;");
  bind_text(s.get(),1,v.id); bind_text(s.get(),2,v.title); sqlite3_bind_double(s.get(),3,v.priority); bind_text(s.get(),4,v.source); bind_text(s.get(),5,v.status);
  check(sqlite3_step(s.get()), db_, "upsert goal");
}

std::vector<PersistentGoal> AgentStore::goals() const {
  Statement s(db_, "SELECT id,title,priority,source,status FROM goals ORDER BY priority DESC,id;");
  std::vector<PersistentGoal> out;
  while (sqlite3_step(s.get()) == SQLITE_ROW) out.push_back({col_text(s.get(),0),col_text(s.get(),1),sqlite3_column_double(s.get(),2),col_text(s.get(),3),col_text(s.get(),4)});
  return out;
}

void AgentStore::set_preference(const PreferenceRecord& v) {
  Statement s(db_, "INSERT INTO preferences(key,weight) VALUES(?,?) ON CONFLICT(key) DO UPDATE SET weight=excluded.weight;");
  bind_text(s.get(),1,v.key); sqlite3_bind_double(s.get(),2,v.weight); check(sqlite3_step(s.get()),db_,"set preference");
}

std::vector<PreferenceRecord> AgentStore::preferences() const {
  Statement s(db_, "SELECT key,weight FROM preferences ORDER BY key;"); std::vector<PreferenceRecord> out;
  while (sqlite3_step(s.get()) == SQLITE_ROW) out.push_back({col_text(s.get(),0),sqlite3_column_double(s.get(),1)}); return out;
}

void AgentStore::set_drive(const DriveRecord& v) {
  Statement s(db_, "INSERT INTO drives(key,level,target,learning_rate) VALUES(?,?,?,?) ON CONFLICT(key) DO UPDATE SET level=excluded.level,target=excluded.target,learning_rate=excluded.learning_rate;");
  bind_text(s.get(),1,v.key); sqlite3_bind_double(s.get(),2,v.level); sqlite3_bind_double(s.get(),3,v.target); sqlite3_bind_double(s.get(),4,v.learning_rate); check(sqlite3_step(s.get()),db_,"set drive");
}

std::vector<DriveRecord> AgentStore::drives() const {
  Statement s(db_, "SELECT key,level,target,learning_rate FROM drives ORDER BY key;"); std::vector<DriveRecord> out;
  while (sqlite3_step(s.get()) == SQLITE_ROW) out.push_back({col_text(s.get(),0),sqlite3_column_double(s.get(),1),sqlite3_column_double(s.get(),2),sqlite3_column_double(s.get(),3)}); return out;
}

void AgentStore::append_reward(const RewardEvent& v) {
  Statement s(db_, "INSERT INTO rewards(id,source,value,reason,created_at) VALUES(?,?,?,?,?);");
  bind_text(s.get(),1,v.id); bind_text(s.get(),2,v.source); sqlite3_bind_double(s.get(),3,v.value); bind_text(s.get(),4,v.reason); bind_text(s.get(),5,v.created_at); check(sqlite3_step(s.get()),db_,"append reward");
}

double AgentStore::reward_total() const {
  Statement s(db_, "SELECT COALESCE(SUM(value),0) FROM rewards;"); return sqlite3_step(s.get()) == SQLITE_ROW ? sqlite3_column_double(s.get(),0) : 0.0;
}

void AgentStore::create_session(const OperatorSession& v) {
  Statement s(db_, "INSERT INTO operator_sessions(id,operator_id,token_hash,expires_unix,revoked) VALUES(?,?,?,?,?);");
  bind_text(s.get(),1,v.id); bind_text(s.get(),2,v.operator_id); bind_text(s.get(),3,v.token_hash); sqlite3_bind_int64(s.get(),4,v.expires_unix); sqlite3_bind_int(s.get(),5,v.revoked?1:0); check(sqlite3_step(s.get()),db_,"create session");
}

std::optional<OperatorSession> AgentStore::find_session(const std::string& id) const {
  Statement s(db_, "SELECT id,operator_id,token_hash,expires_unix,revoked FROM operator_sessions WHERE id=?;"); bind_text(s.get(),1,id);
  if (sqlite3_step(s.get()) != SQLITE_ROW) return std::nullopt;
  return OperatorSession{col_text(s.get(),0),col_text(s.get(),1),col_text(s.get(),2),sqlite3_column_int64(s.get(),3),sqlite3_column_int(s.get(),4)!=0};
}

void AgentStore::revoke_session(const std::string& id) {
  Statement s(db_, "UPDATE operator_sessions SET revoked=1 WHERE id=?;"); bind_text(s.get(),1,id); check(sqlite3_step(s.get()),db_,"revoke session");
}

void AgentStore::append_thought(const std::string& id,const std::string& subject,double score,const std::string& reason,const std::string& status,const std::string& created_at) {
  Statement s(db_, "INSERT OR REPLACE INTO thoughts(id,subject,score,reason,status,created_at) VALUES(?,?,?,?,?,?);");
  bind_text(s.get(),1,id); bind_text(s.get(),2,subject); sqlite3_bind_double(s.get(),3,score); bind_text(s.get(),4,reason); bind_text(s.get(),5,status); bind_text(s.get(),6,created_at); check(sqlite3_step(s.get()),db_,"append thought");
}

}  // namespace exotic::cognition
