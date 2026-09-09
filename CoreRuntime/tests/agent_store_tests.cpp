#include "exotic/cognition/agent_store.hpp"
#include "exotic/cognition/idle_cognition.hpp"
#include "exotic/cognition/operator_sessions.hpp"

#include <cassert>
#include <cstdio>
#include <string>

int main() {
  const std::string path = "exotic_agent_store_test.db";
  std::remove(path.c_str());

  {
    exotic::cognition::AgentStore store(path);
    store.initialize();
    store.save_identity({"free-agent-01", "Nova", "Persistent autonomous EXOTIC agent", "geometric", "hex-eye"});
    store.upsert_belief({"belief-1", "Verification should precede external action", 0.97, "ActionGate policy", "2026-09-09T00:00:00Z"});
    store.upsert_goal({"goal-1", "Investigate unresolved high-value questions", 0.90, "self", "active"});
    store.set_preference({"novelty", 0.60});
    store.set_drive({"curiosity", 0.40, 0.75, 0.10});
    store.append_reward({"reward-1", "verification", 0.50, "Correctly deferred unauthorized action", "2026-09-09T00:00:00Z"});

    exotic::cognition::IdleCognitionScheduler idle(store);
    assert(idle.should_run(false));
    const auto candidate = idle.next_candidate();
    assert(candidate.has_value());
    assert(candidate->subject.find("Advance goal") != std::string::npos);

    exotic::cognition::OperatorSessionManager sessions(store);
    const auto session = sessions.create("local-operator", "0123456789abcdef0123456789abcdef", 1000, 60);
    assert(sessions.validate(session.id, "0123456789abcdef0123456789abcdef", 1010));
    assert(!sessions.validate(session.id, "wrong-wrong-wrong-wrong", 1010));
    sessions.revoke(session.id);
    assert(!sessions.validate(session.id, "0123456789abcdef0123456789abcdef", 1010));
  }

  {
    exotic::cognition::AgentStore store(path);
    store.initialize();
    const auto identity = store.load_identity();
    assert(identity.name == "Nova");
    assert(identity.avatar_value == "hex-eye");
    assert(store.beliefs().size() == 1);
    assert(store.goals().size() == 1);
    assert(store.preferences().size() == 1);
    assert(store.drives().size() == 1);
    assert(store.reward_total() == 0.50);
  }

  std::remove(path.c_str());
  std::remove((path + "-wal").c_str());
  std::remove((path + "-shm").c_str());
  return 0;
}
