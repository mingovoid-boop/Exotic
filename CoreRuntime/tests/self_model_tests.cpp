#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/self_model.hpp"

#include <cassert>
#include <cstdio>

int main() {
  const char* db = "self_model_test.sqlite";
  std::remove(db);
  exotic::cognition::AgentStore store(db);
  store.initialize();
  store.save_identity({"free-agent-01","EVA","Learns from bounded experience.","geometric","hex-eye"});
  store.upsert_belief({"b1","A recurring failure pattern has a common cause",0.42,"partial logs","2026-09-09T00:00:00Z"});
  store.set_preference({"verification",0.60});
  store.set_drive({"curiosity",0.30,0.75,0.05});

  exotic::cognition::DevelopmentalSelfModel self(store);
  self.remember({"e1","Investigating logs revealed a reproducible failure signature.",0.9,0.5,"2026-09-09T00:01:00Z"});
  self.remember({"e2","A proposed fix failed its verification test.",0.8,-0.4,"2026-09-09T00:02:00Z"});
  self.consolidate();
  self.reflect();
  self.adapt_preferences(0.05);
  self.generate_candidate_goals();

  const auto snap = self.snapshot();
  assert(snap.identity.name == "EVA");
  assert(!snap.traits.empty());
  for (const auto& t : snap.traits) {
    assert(t.value >= 0.0 && t.value <= 1.0);
    assert(t.plasticity >= 0.0 && t.plasticity <= 0.25);
  }
  assert(!snap.unresolved_questions.empty());

  bool found_self_goal = false;
  for (const auto& g : store.goals()) {
    if (g.source == "self") found_self_goal = true;
    assert(g.priority >= 0.0 && g.priority <= 1.0);
  }
  assert(found_self_goal);

  for (const auto& p : store.preferences()) assert(p.weight >= -1.0 && p.weight <= 1.0);
  std::remove(db);
  std::remove("self_model_test.sqlite-wal");
  std::remove("self_model_test.sqlite-shm");
  return 0;
}
