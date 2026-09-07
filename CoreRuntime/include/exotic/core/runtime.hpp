#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace exotic::core {

enum class Risk { low, medium, high };
enum class Authority { none, read, sandbox, external };

struct Event {
    std::uint64_t sequence{};
    std::string type;
    std::string actor;
    std::string subject;
    std::string payload;
    std::chrono::system_clock::time_point timestamp;
    std::string previous_hash;
    std::string hash;
};

class EventStore {
public:
    explicit EventStore(std::string journal_path = {});
    const Event& append(std::string type, std::string actor, std::string subject, std::string payload);
    const std::vector<Event>& all() const noexcept { return events_; }
    bool verify_chain() const;
    bool durable() const noexcept { return !journal_path_.empty(); }
private:
    void load();
    void persist(const Event& event) const;
    std::string journal_path_;
    std::vector<Event> events_;
};

struct Capability {
    std::string id;
    Authority authority{Authority::none};
    Risk risk{Risk::low};
    bool enabled{true};
};

class CapabilityRegistry {
public:
    bool register_capability(Capability capability);
    const Capability* find(const std::string& id) const;
    std::size_t size() const noexcept { return capabilities_.size(); }
private:
    std::unordered_map<std::string, Capability> capabilities_;
};

struct Identity {
    std::string id;
    bool enabled{true};
    std::vector<std::string> grants;
};

class IdentityRegistry {
public:
    bool register_identity(Identity identity);
    bool grant(const std::string& identity_id, const std::string& capability_id);
    bool allowed(const std::string& identity_id, const std::string& capability_id) const;
private:
    std::unordered_map<std::string, Identity> identities_;
};

struct ActionRequest {
    std::string objective;
    std::string capability_id;
    std::string input;
    bool simulation{true};
    std::string actor_id{"operator"};
};

struct PolicyDecision { bool allowed{false}; std::string reason; };
class PolicyEngine {
public:
    PolicyDecision authorize(const ActionRequest& request, const Capability& capability,
                             const IdentityRegistry& identities) const;
};

struct Verification { bool passed{false}; std::string evidence; };
struct ActionResult {
    bool executed{false};
    bool committed{false};
    std::string output;
    Verification verification;
};

class Runtime {
public:
    explicit Runtime(std::string journal_path = {});
    ActionResult run(const ActionRequest& request);
    const EventStore& events() const noexcept { return events_; }
    const CapabilityRegistry& capabilities() const noexcept { return registry_; }
    IdentityRegistry& identities() noexcept { return identities_; }
private:
    EventStore events_;
    CapabilityRegistry registry_;
    IdentityRegistry identities_;
    PolicyEngine policy_;
};

} // namespace exotic::core
