#include "exotic/core/runtime.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace exotic::core {
namespace {
std::string digest(const Event& e) {
    const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(e.timestamp.time_since_epoch()).count();
    const std::string material = std::to_string(e.sequence) + "|" + e.type + "|" + e.actor + "|" +
        e.subject + "|" + e.payload + "|" + std::to_string(ticks) + "|" + e.previous_hash;
    const auto value = std::hash<std::string>{}(material);
    std::ostringstream out;
    out << std::hex << std::setw(sizeof(value) * 2) << std::setfill('0') << value;
    return out.str();
}

std::string escape(std::string value) {
    std::string out;
    for (char c : value) {
        if (c == '\\' || c == '\t' || c == '\n') { out += '\\'; out += (c == '\t' ? 't' : c == '\n' ? 'n' : '\\'); }
        else out += c;
    }
    return out;
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> fields(1);
    bool escaped = false;
    for (char c : line) {
        if (escaped) { fields.back() += c == 't' ? '\t' : c == 'n' ? '\n' : c; escaped = false; }
        else if (c == '\\') escaped = true;
        else if (c == '\t') fields.emplace_back();
        else fields.back() += c;
    }
    return fields;
}
} // namespace

EventStore::EventStore(std::string journal_path) : journal_path_(std::move(journal_path)) { load(); }

const Event& EventStore::append(std::string type, std::string actor, std::string subject, std::string payload) {
    Event event{static_cast<std::uint64_t>(events_.size() + 1), std::move(type), std::move(actor),
                std::move(subject), std::move(payload), std::chrono::system_clock::now(),
                events_.empty() ? "GENESIS" : events_.back().hash, {}};
    event.hash = digest(event);
    events_.push_back(event);
    persist(events_.back());
    return events_.back();
}

void EventStore::persist(const Event& e) const {
    if (journal_path_.empty()) return;
    std::ofstream out(journal_path_, std::ios::app);
    if (!out) throw std::runtime_error("cannot open EXOTIC event journal");
    const auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(e.timestamp.time_since_epoch()).count();
    out << e.sequence << '\t' << ticks << '\t' << escape(e.type) << '\t' << escape(e.actor) << '\t'
        << escape(e.subject) << '\t' << escape(e.payload) << '\t' << e.previous_hash << '\t' << e.hash << '\n';
}

void EventStore::load() {
    if (journal_path_.empty()) return;
    std::ifstream in(journal_path_);
    if (!in) return;
    std::string line;
    while (std::getline(in, line)) {
        const auto f = split(line);
        if (f.size() != 8) throw std::runtime_error("corrupt EXOTIC event journal record");
        Event e;
        e.sequence = std::stoull(f[0]);
        e.timestamp = std::chrono::system_clock::time_point(std::chrono::milliseconds(std::stoll(f[1])));
        e.type=f[2]; e.actor=f[3]; e.subject=f[4]; e.payload=f[5]; e.previous_hash=f[6]; e.hash=f[7];
        events_.push_back(std::move(e));
    }
    if (!verify_chain()) throw std::runtime_error("EXOTIC event journal integrity failure");
}

bool EventStore::verify_chain() const {
    for (std::size_t i = 0; i < events_.size(); ++i) {
        const auto& e = events_[i];
        if (e.sequence != i + 1) return false;
        if (e.previous_hash != (i == 0 ? "GENESIS" : events_[i - 1].hash)) return false;
        if (e.hash != digest(e)) return false;
    }
    return true;
}

bool CapabilityRegistry::register_capability(Capability capability) {
    if (capability.id.empty()) return false;
    return capabilities_.emplace(capability.id, std::move(capability)).second;
}
const Capability* CapabilityRegistry::find(const std::string& id) const {
    const auto it = capabilities_.find(id); return it == capabilities_.end() ? nullptr : &it->second;
}

bool IdentityRegistry::register_identity(Identity identity) {
    if (identity.id.empty()) return false;
    return identities_.emplace(identity.id, std::move(identity)).second;
}
bool IdentityRegistry::grant(const std::string& identity_id, const std::string& capability_id) {
    auto it = identities_.find(identity_id); if (it == identities_.end()) return false;
    if (std::find(it->second.grants.begin(), it->second.grants.end(), capability_id) == it->second.grants.end())
        it->second.grants.push_back(capability_id);
    return true;
}
bool IdentityRegistry::allowed(const std::string& identity_id, const std::string& capability_id) const {
    const auto it = identities_.find(identity_id);
    return it != identities_.end() && it->second.enabled &&
        std::find(it->second.grants.begin(), it->second.grants.end(), capability_id) != it->second.grants.end();
}

bool ApprovalRegistry::issue(Approval approval) {
    if (approval.id.empty() || approval.actor_id.empty() || approval.capability_id.empty()) return false;
    return approvals_.emplace(approval.id, std::move(approval)).second;
}
bool ApprovalRegistry::consume(const std::string& approval_id, const std::string& actor_id, const std::string& capability_id) {
    auto it = approvals_.find(approval_id);
    if (it == approvals_.end() || it->second.consumed) return false;
    if (it->second.actor_id != actor_id || it->second.capability_id != capability_id) return false;
    it->second.consumed = true;
    return true;
}

void ResourceGovernor::set_budget(const std::string& actor_id, std::uint64_t units) { budgets_[actor_id] = units; }
bool ResourceGovernor::reserve(const std::string& actor_id, std::uint64_t units) {
    auto it = budgets_.find(actor_id);
    if (it == budgets_.end() || units == 0 || it->second < units) return false;
    it->second -= units;
    return true;
}
std::uint64_t ResourceGovernor::remaining(const std::string& actor_id) const {
    const auto it = budgets_.find(actor_id); return it == budgets_.end() ? 0 : it->second;
}

PolicyDecision PolicyEngine::authorize(const ActionRequest& request, const Capability& capability,
                                       const IdentityRegistry& identities, ApprovalRegistry& approvals) const {
    if (!capability.enabled) return {false, "capability disabled"};
    if (!identities.allowed(request.actor_id, capability.id)) return {false, "actor lacks capability grant"};
    if (capability.risk == Risk::high && !approvals.consume(request.approval_id, request.actor_id, capability.id))
        return {false, "high-risk capability requires valid single-use approval"};
    if (!request.simulation && capability.authority == Authority::external)
        return {false, "external side effects disabled in v0.3"};
    return {true, request.simulation ? "simulation authorized" : "authorized"};
}

Runtime::Runtime(std::string journal_path) : events_(std::move(journal_path)) {
    registry_.register_capability({"core.observe", Authority::read, Risk::low, true});
    registry_.register_capability({"core.plan", Authority::none, Risk::low, true});
    registry_.register_capability({"core.execute.echo", Authority::sandbox, Risk::low, true});
    identities_.register_identity({"operator", true, {"core.observe", "core.plan", "core.execute.echo"}});
    resources_.set_budget("operator", 100);
}

ActionResult Runtime::run(const ActionRequest& request) {
    events_.append("objective.received", request.actor_id, request.capability_id, request.objective);
    const Capability* capability = registry_.find(request.capability_id);
    if (!capability) {
        events_.append("policy.denied", "policy", request.capability_id, "unknown capability");
        return {false,false,{}, {false,"unknown capability"}};
    }

    const auto decision = policy_.authorize(request, *capability, identities_, approvals_);
    events_.append(decision.allowed ? "policy.allowed" : "policy.denied", "policy", capability->id, decision.reason);
    if (!decision.allowed) return {false,false,{}, {false,decision.reason}};

    if (!resources_.reserve(request.actor_id, request.cost_units)) {
        events_.append("budget.denied", "resource-governor", capability->id, "insufficient budget");
        return {false,false,{}, {false,"insufficient budget"}};
    }
    events_.append("budget.reserved", "resource-governor", capability->id, std::to_string(request.cost_units));

    std::string output;
    if (capability->id == "core.execute.echo") output=request.input;
    else if (capability->id == "core.observe") output="observed:"+request.input;
    else if (capability->id == "core.plan") output="plan:"+request.objective;

    events_.append(request.simulation ? "execution.simulated" : "execution.completed", "executor", capability->id, output);
    Verification verification{!output.empty(), !output.empty() ? "non-empty deterministic output" : "empty output"};
    events_.append(verification.passed ? "verification.passed" : "verification.failed", "verifier", capability->id, verification.evidence);
    const bool committed=verification.passed;
    if (committed) events_.append("state.committed", "runtime", capability->id, "verified result accepted");
    return {true,committed,std::move(output),std::move(verification)};
}

} // namespace exotic::core
