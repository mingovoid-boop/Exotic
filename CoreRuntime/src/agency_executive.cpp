#include "exotic/cognition/agency_executive.hpp"
#include "exotic/cognition/stable_id.hpp"
#include "exotic/cognition/storage_schema.hpp"
#include <sqlite3.h>
#include <stdexcept>
namespace exotic::cognition {namespace {const char*outcome_name(VolitionOutcome o){switch(o){case VolitionOutcome::Choose:return"choose";case VolitionOutcome::Defer:return"defer";case VolitionOutcome::Reject:return"reject";default:return"do_nothing";}}}
AgencyCycle AgencyExecutive::consider(const AgencyInput& input) const {
  AgencyCycle c;c.id=stable_id("agency-cycle");
  c.volition=volition_.decide(input.options);
  c.metacognition=meta_.consider(input.subject,input.uncertainty,input.stakes);
  c.arbitration=arbiter_.consider(input.intention_demands,1.0);
  c.timing=temporal_.consider(input.temporal_options,input.now);
  for(const auto&r:input.reconsiderations)c.reconsideration.push_back(reconsideration_.consider(r.current,r.signal,r.alternative));
  c.external_action_authorized=gate_.authorize(CognitiveAction::Act,input.external_action_requested&&input.explicit_external_grant);
  if(input.external_action_requested&&!c.external_action_authorized)c.rationale="External action remains unavailable without explicit grant; internal review remains inspectable and non-mutating.";
  else if(c.volition.outcome==VolitionOutcome::DoNothing)c.rationale="The executive considered volition, timing, conflicts, and reconsideration while preserving the option to do nothing.";
  else c.rationale="The executive assembled one voluntary decision cycle; recommendations do not automatically commit, reschedule, revise, or act.";
  return c;
}
ChoiceHistoryRecord AgencyExecutive::remember_choice(const std::string&subject,const VolitionDecision&decision,const std::string&created_at,std::string supersedes){return intentions_.record_choice(subject,decision,created_at,std::move(supersedes));}
bool AgencyExecutive::persist_cycle(const AgencyInput&input,const AgencyCycle&cycle,const std::string&created_at)const{sqlite3*d=nullptr;if(sqlite3_open(store_.path().c_str(),&d)!=SQLITE_OK){if(d)sqlite3_close(d);return false;}try{apply_storage_migrations(d);}catch(...){sqlite3_close(d);throw;}sqlite3_stmt*s=nullptr;const char*q="INSERT INTO agency_cycles(id,subject,volition_outcome,metacognition_engaged,arbitration_count,timing_count,reconsideration_count,external_action_requested,external_action_authorized,rationale,created_at) VALUES(?,?,?,?,?,?,?,?,?,?,?)";if(sqlite3_prepare_v2(d,q,-1,&s,nullptr)!=SQLITE_OK){sqlite3_close(d);return false;}auto bind=[&](int i,const std::string&v){sqlite3_bind_text(s,i,v.c_str(),-1,SQLITE_TRANSIENT);};bind(1,cycle.id);bind(2,input.subject);bind(3,outcome_name(cycle.volition.outcome));sqlite3_bind_int(s,4,cycle.metacognition.engage?1:0);sqlite3_bind_int64(s,5,static_cast<sqlite3_int64>(cycle.arbitration.assignments.size()));sqlite3_bind_int64(s,6,static_cast<sqlite3_int64>(cycle.timing.size()));sqlite3_bind_int64(s,7,static_cast<sqlite3_int64>(cycle.reconsideration.size()));sqlite3_bind_int(s,8,input.external_action_requested?1:0);sqlite3_bind_int(s,9,cycle.external_action_authorized?1:0);bind(10,cycle.rationale);bind(11,created_at);bool ok=sqlite3_step(s)==SQLITE_DONE;sqlite3_finalize(s);sqlite3_close(d);return ok;}
} // namespace exotic::cognition
