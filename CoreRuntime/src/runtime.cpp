#include "exotic/core/runtime.hpp"

#include <utility>

namespace exotic::core {

const Event& EventStore::append(std::string type, std::string actor, std::string subject, std::string payload) {
    events_.push_back(Event{
        static_cast<std::uint64_t>(events_.size() + 1),
        std::move(type),
        std::move(actor),
        std::move(subject),
        std::move(payload),
        std::chrono::system_clock::now()
    });
    return events_.back();
}

bool CapabilityRegistry::register_capability(Capability capability) {
    if (capability.id.empty()) return false;
    return capabilities_.emplace(capability.id, std::move(capability)).second;
}

const Capability* CapabilityRegistry::find(const std::string& id) const {
    const auto it = capabilities_.find(id);
    return it == capabilities_.end() ? nullptr : &it->second;
}

PolicyDecision PolicyEngine::authorize(const ActionRequest& request, const Capability& capability) const {
    if (!capability.enabled) return {false, "capability disabled"};
    if (capability.risk == Risk::high) return {false, "high-risk capability requires explicit approval"};
    if (!request.simulation && capability.authority == Authority::external)
        return {false, "external side effects disabled in v0.1"};
    return {true, request.simulation ? "simulation authorized" : "authorized"};
}

Runtime::Runtime() {
    registry_.register_capability({"core.observe", Authority::read, Risk::low, true});
    registry_.register_capability({"core.plan", Authority::none, Risk::low, true});
    registry_.register_capability({"core.execute.echo", Authority::sandbox, Risk::low, true});
}

ActionResult Runtime::run(const ActionRequest& request) {
    events_.append("objective.received", "runtime", request.capability_id, request.objective);

    const Capability* capability = registry_.find(request.capability_id);
    if (!capability) {
        events_.append("policy.denied", "policy", request.capability_id, "unknown capability");
        return {false, false, {}, {false, "unknown capability"}};
    }

    const PolicyDecision decision = policy_.authorize(request, *capability);
    events_.append(decision.allowed ? "policy.allowed" : "policy.denied", "policy", capability->id, decision.reason);
    if (!decision.allowed) return {false, false, {}, {false, decision.reason}};

    std::string output;
    if (capability->id == "core.execute.echo") output = request.input;
    else if (capability->id == "core.observe") output = "observed:" + request.input;
    else if (capability->id == "core.plan") output = "plan:" + request.objective;

    events_.append(request.simulation ? "execution.simulated" : "execution.completed", "executor", capability->id, output);

    Verification verification{!output.empty(), !output.empty() ? "non-empty deterministic output" : "empty output"};
    events_.append(verification.passed ? "verification.passed" : "verification.failed", "verifier", capability->id, verification.evidence);

    const bool committed = verification.passed;
    if (committed) events_.append("state.committed", "runtime", capability->id, "verified result accepted");

    return {true, committed, std::move(output), std::move(verification)};
}

} // namespace exotic::core
