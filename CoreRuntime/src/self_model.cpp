#include "exotic/cognition/self_model.hpp"

#include <sqlite3.h>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace exotic::cognition {
namespace {

void check(int rc, sqlite3* db, const char* what) {
  if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) throw std::runtime_error(std::string(what)+": "+sqlite3_errmsg(db));
}

class Db {
 public:
  explicit Db(const std::string& path) { check(sqlite3_open(path.c_str(), &db_), db_, "open self-model db"); ensure(); }
  ~Db() { if (db_) sqlite3_close(db_); }
  sqlite3* get() const { return db_; }
  void exec(const char* sql) { char* err=nullptr; const int rc=sqlite3_exec(db_,sql,nullptr,nullptr,&err); if(rc!=SQLITE_OK){std::string m=err?err:sqlite3_errmsg(db_); sqlite3_free(err); throw std::runtime_error(m);} }
 private:
  void ensure() {
    exec("CREATE TABLE IF NOT EXISTS episodes(id TEXT PRIMARY KEY,summary TEXT NOT NULL,salience REAL NOT NULL,reward REAL NOT NULL,created_at TEXT NOT NULL);"
         "CREATE TABLE IF NOT EXISTS semantic_memory(id TEXT PRIMARY KEY,concept TEXT NOT NULL,statement TEXT NOT NULL,confidence REAL NOT NULL,source_episode TEXT NOT NULL);"
         "CREATE TABLE IF NOT EXISTS personality_traits(key TEXT PRIMARY KEY,value REAL NOT NULL CHECK(value BETWEEN 0 AND 1),plasticity REAL NOT NULL CHECK(plasticity BETWEEN 0 AND 0.25));"
         "CREATE TABLE IF NOT EXISTS reflections(id TEXT PRIMARY KEY,observation TEXT NOT NULL,lesson TEXT NOT NULL,confidence REAL NOT NULL,created_at TEXT NOT NULL);"
         "CREATE INDEX IF NOT EXISTS idx_episodes_created ON episodes(created_at DESC);"
         "CREATE INDEX IF NOT EXISTS idx_semantic_confidence ON semantic_memory(confidence DESC);");
  }
  sqlite3* db_{};
};

class Stmt {
 public: Stmt(sqlite3* db,const char* sql):db_(db){check(sqlite3_prepare_v2(db,sql,-1,&s_,nullptr),db,"prepare");} ~Stmt(){sqlite3_finalize(s_);} sqlite3_stmt* get()const{return s_;}
 private: sqlite3* db_{}; sqlite3_stmt* s_{};
};

void text(sqlite3_stmt* s,int i,const std::string& v){sqlite3_bind_text(s,i,v.c_str(),static_cast<int>(v.size()),SQLITE_TRANSIENT);} 
std::string col(sqlite3_stmt* s,int i){const auto* p=sqlite3_column_text(s,i);return p?reinterpret_cast<const char*>(p):"";}
std::string now_iso(){const auto now=std::chrono::system_clock::now();const std::time_t t=std::chrono::system_clock::to_time_t(now);std::tm tm{};
#ifdef _WIN32
  gmtime_s(&tm,&t);
#else
  gmtime_r(&t,&tm);
#endif
  std::ostringstream o;o<<std::put_time(&tm,"%Y-%m-%dT%H:%M:%SZ");return o.str();}
std::string slug(std::string s){for(char& c:s){if(!std::isalnum(static_cast<unsigned char>(c)))c='-';else c=static_cast<char>(std::tolower(static_cast<unsigned char>(c)));} if(s.size()>40)s.resize(40);return s;}

double clamp01(double v){return std::clamp(v,0.0,1.0);} 

std::vector<Episode> load_episodes(sqlite3* db,std::size_t limit){Stmt q(db,"SELECT id,summary,salience,reward,created_at FROM episodes ORDER BY created_at DESC LIMIT ?;");sqlite3_bind_int64(q.get(),1,static_cast<sqlite3_int64>(limit));std::vector<Episode> out;while(sqlite3_step(q.get())==SQLITE_ROW)out.push_back({col(q.get(),0),col(q.get(),1),sqlite3_column_double(q.get(),2),sqlite3_column_double(q.get(),3),col(q.get(),4)});return out;}
std::vector<PersonalityTrait> load_traits(sqlite3* db){Stmt q(db,"SELECT key,value,plasticity FROM personality_traits ORDER BY key;");std::vector<PersonalityTrait> out;while(sqlite3_step(q.get())==SQLITE_ROW)out.push_back({col(q.get(),0),sqlite3_column_double(q.get(),1),sqlite3_column_double(q.get(),2)});return out;}
void save_trait(sqlite3* db,const PersonalityTrait& t){Stmt q(db,"INSERT INTO personality_traits(key,value,plasticity) VALUES(?,?,?) ON CONFLICT(key) DO UPDATE SET value=excluded.value,plasticity=excluded.plasticity;");text(q.get(),1,t.key);sqlite3_bind_double(q.get(),2,clamp01(t.value));sqlite3_bind_double(q.get(),3,std::clamp(t.plasticity,0.0,0.25));check(sqlite3_step(q.get()),db,"save trait");}

}  // namespace

void DevelopmentalSelfModel::remember(const Episode& e) {
  Db db(store_.path());
  Stmt q(db.get(),"INSERT OR REPLACE INTO episodes(id,summary,salience,reward,created_at) VALUES(?,?,?,?,?);");
  text(q.get(),1,e.id);text(q.get(),2,e.summary);sqlite3_bind_double(q.get(),3,clamp01(e.salience));sqlite3_bind_double(q.get(),4,std::clamp(e.reward,-1.0,1.0));text(q.get(),5,e.created_at.empty()?now_iso():e.created_at);check(sqlite3_step(q.get()),db.get(),"remember episode");
}

void DevelopmentalSelfModel::consolidate() {
  Db db(store_.path());
  for(const auto& e:load_episodes(db.get(),32)) {
    if(e.salience<0.60 && std::abs(e.reward)<0.25) continue;
    const double conf=std::clamp(0.45+0.35*e.salience+0.15*std::abs(e.reward),0.50,0.95);
    Stmt q(db.get(),"INSERT INTO semantic_memory(id,concept,statement,confidence,source_episode) VALUES(?,?,?,?,?) ON CONFLICT(id) DO UPDATE SET statement=excluded.statement,confidence=MAX(semantic_memory.confidence,excluded.confidence),source_episode=excluded.source_episode;");
    const std::string id="semantic-"+slug(e.id);text(q.get(),1,id);text(q.get(),2,"experience");text(q.get(),3,e.summary);sqlite3_bind_double(q.get(),4,conf);text(q.get(),5,e.id);check(sqlite3_step(q.get()),db.get(),"consolidate memory");
  }
}

void DevelopmentalSelfModel::reflect() {
  Db db(store_.path()); const auto eps=load_episodes(db.get(),8); if(eps.empty()) return;
  double reward=0,salience=0; for(const auto& e:eps){reward+=e.reward;salience+=e.salience;} reward/=eps.size();salience/=eps.size();
  const std::string lesson=reward>=0.15?"Recent strategies produced useful outcomes; preserve the productive pattern.":reward<=-0.15?"Recent strategies underperformed; increase verification and reconsider assumptions.":"Recent outcomes are mixed; gather more evidence before changing strategy.";
  const auto stamp=now_iso(); Stmt q(db.get(),"INSERT OR REPLACE INTO reflections(id,observation,lesson,confidence,created_at) VALUES(?,?,?,?,?);");text(q.get(),1,"reflection-"+slug(stamp));text(q.get(),2,"Reviewed "+std::to_string(eps.size())+" recent episodes.");text(q.get(),3,lesson);sqlite3_bind_double(q.get(),4,std::clamp(0.55+0.25*salience,0.55,0.85));text(q.get(),5,stamp);check(sqlite3_step(q.get()),db.get(),"reflect");

  auto traits=load_traits(db.get()); if(traits.empty()) traits={{"curiosity",0.65,0.04},{"persistence",0.60,0.03},{"caution",0.50,0.04},{"adaptability",0.60,0.04}};
  for(auto t:traits){double signal=0.0;if(t.key=="persistence")signal=reward;if(t.key=="caution")signal=-reward;if(t.key=="adaptability")signal=std::abs(reward);if(t.key=="curiosity")signal=1.0-salience;t.value=clamp01(t.value+t.plasticity*std::clamp(signal,-1.0,1.0));save_trait(db.get(),t);}
}

void DevelopmentalSelfModel::adapt_preferences(double max_delta) {
  max_delta=std::clamp(max_delta,0.0,0.10); const auto eps=[&](){Db db(store_.path());return load_episodes(db.get(),12);}(); if(eps.empty())return;
  double avg=0;for(const auto&e:eps)avg+=e.reward;avg/=eps.size();
  for(auto p:store_.preferences()){const double delta=std::clamp(avg*0.05,-max_delta,max_delta);p.weight=std::clamp(p.weight+delta,-1.0,1.0);store_.set_preference(p);}
}

void DevelopmentalSelfModel::generate_candidate_goals() {
  for(const auto& b:store_.beliefs()) if(b.confidence<0.60){store_.upsert_goal({"self-belief-"+slug(b.id),"Resolve uncertainty: "+b.proposition,std::clamp(0.55+(0.60-b.confidence),0.55,0.85),"self","active"});break;}
  for(const auto& d:store_.drives()) if(std::abs(d.target-d.level)>=0.20){store_.upsert_goal({"self-drive-"+slug(d.key),"Improve "+d.key+" toward target",std::clamp(0.50+std::abs(d.target-d.level),0.50,0.80),"self","active"});break;}
}

SelfModelSnapshot DevelopmentalSelfModel::snapshot() const {
  Db db(store_.path()); SelfModelSnapshot s; s.identity=store_.load_identity(); s.traits=load_traits(db.get());
  for(const auto& g:store_.goals()) if(g.status=="active"&&g.priority>=0.80)s.commitments.push_back(g.title);
  for(const auto& b:store_.beliefs()) if(b.confidence<0.65)s.unresolved_questions.push_back(b.proposition);
  std::ostringstream n;n<<s.identity.name<<" has "<<s.traits.size()<<" adaptive traits, "<<s.commitments.size()<<" high-priority commitments, and "<<s.unresolved_questions.size()<<" unresolved beliefs.";s.narrative=n.str();return s;
}

}  // namespace exotic::cognition
