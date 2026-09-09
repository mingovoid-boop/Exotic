#include "exotic/cognition/free_agent_service.hpp"

#include <cassert>
#include <filesystem>
#include <string>

int main() {
  const auto journal = (std::filesystem::temp_directory_path() / "exotic-free-agent-service-test.tsv").string();
  std::error_code ec;
  std::filesystem::remove(journal, ec);

  {
    exotic::cognition::FreeAgentService service{journal};

    const auto health = service.health_json();
    assert(health.find("\"status\":\"ok\"") != std::string::npos);

    const auto initial = service.state_json();
    assert(initial.find("free-agent-01") != std::string::npos);
    assert(initial.find("\"autonomy\":1.0") != std::string::npos);
    assert(initial.find("\"externalAuthority\":0.0") != std::string::npos);

    const auto thought = service.submit_thought("Investigate runtime\tpersistence\nacross restarts");
    assert(thought.id == "thought-1");
    assert(thought.score > 0.0);
    assert(thought.status == "selected");

    const auto after = service.state_json();
    assert(after.find("Investigate runtime\\tpersistence\\nacross restarts") != std::string::npos);
    assert(after.find("\"mode\":\"thinking\"") != std::string::npos);

    assert(service.set_mode("waiting"));
    assert(service.mode() == "waiting");
    assert(!service.set_mode("unbounded"));
    assert(service.mode() == "waiting");
  }

  {
    exotic::cognition::FreeAgentService reloaded{journal};
    const auto restored = reloaded.state_json();
    assert(restored.find("Investigate runtime\\tpersistence\\nacross restarts") != std::string::npos);
    const auto second = reloaded.submit_thought("Second durable thought");
    assert(second.id == "thought-2");
  }

  std::filesystem::remove(journal, ec);
  return 0;
}
