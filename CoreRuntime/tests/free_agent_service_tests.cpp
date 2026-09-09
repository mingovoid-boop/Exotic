#include "exotic/cognition/free_agent_service.hpp"

#include <cassert>
#include <string>

int main() {
  exotic::cognition::FreeAgentService service;

  const auto health = service.health_json();
  assert(health.find("\"status\":\"ok\"") != std::string::npos);

  const auto initial = service.state_json();
  assert(initial.find("free-agent-01") != std::string::npos);
  assert(initial.find("\"autonomy\":1.0") != std::string::npos);
  assert(initial.find("\"externalAuthority\":0.0") != std::string::npos);

  const auto thought = service.submit_thought("Investigate runtime persistence");
  assert(thought.id == "thought-1");
  assert(thought.subject == "Investigate runtime persistence");
  assert(thought.score > 0.0);
  assert(thought.status == "selected");

  const auto after = service.state_json();
  assert(after.find("Investigate runtime persistence") != std::string::npos);
  assert(after.find("\"mode\":\"thinking\"") != std::string::npos);

  assert(service.set_mode("waiting"));
  assert(service.mode() == "waiting");
  assert(!service.set_mode("unbounded"));
  assert(service.mode() == "waiting");

  return 0;
}
