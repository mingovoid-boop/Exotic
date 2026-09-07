#include "exotic/core/runtime.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace exotic::core;

static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error("TEST FAILURE: " + message);
}

static void test_known_capability_executes_and_verifies() {
    Runtime runtime;
    auto result = runtime.run({"echo test", "core.execute.echo", "hello", true});
    require(result.executed, "known capability did not execute");
    require(result.verification.passed, "known capability did not verify");
    require(result.committed, "verified result did not commit");
    require(result.output == "hello", "echo output mismatch");
    const auto& events = runtime.events().all();
    require(events.size() == 6, "unexpected event count");
    require(events[0].type == "objective.received", "missing objective event");
    require(events[1].type == "policy.allowed", "missing policy authorization event");
    require(events[2].type == "budget.reserved", "missing budget reservation event");
    require(events[3].type == "execution.simulated", "missing simulation event");
    require(events[4].type == "verification.passed", "missing verification event");
    require(events[5].type == "state.committed", "missing commit event");
    require(runtime.events().verify_chain(), "event chain invalid");
}

static void test_unknown_capability_is_denied() {
    Runtime runtime;
    auto result = runtime.run({"deny test", "missing.capability", "x", true});
    require(!result.executed, "unknown capability executed");
    require(!result.committed, "unknown capability committed");
    require(!result.verification.passed, "unknown capability verified");
    require(runtime.events().all().size() == 2, "denial evidence count mismatch");
}

static void test_ungranted_actor_is_denied() {
    Runtime runtime;
    runtime.identities().register_identity({"guest", true, {"core.observe"}});
    auto result = runtime.run({"unauthorized", "core.execute.echo", "x", true, "guest"});
    require(!result.executed, "ungranted actor executed capability");
    require(!result.committed, "ungranted actor committed state");
    require(result.verification.evidence == "actor lacks capability grant", "wrong authorization denial reason");
}

static void test_budget_denial_blocks_execution() {
    Runtime runtime;
    runtime.resources().set_budget("operator", 1);
    auto result = runtime.run({"over budget", "core.execute.echo", "x", true, "operator", "", 2});
    require(!result.executed, "over-budget action executed");
    require(!result.committed, "over-budget action committed");
    require(result.verification.evidence == "resource budget denied", "wrong budget denial reason");
    const auto& events = runtime.events().all();
    require(events.size() == 3 && events.back().type == "budget.denied", "budget denial not audited");
}

static void test_event_sequences_are_monotonic() {
    Runtime runtime;
    runtime.run({"one", "core.execute.echo", "1", true});
    runtime.run({"two", "core.execute.echo", "2", true});
    const auto& events = runtime.events().all();
    for (std::size_t i = 0; i < events.size(); ++i)
        require(events[i].sequence == i + 1, "event sequence is not monotonic");
    require(runtime.events().verify_chain(), "multi-action event chain invalid");
}

static void test_journal_survives_restart() {
    const char* path = "exotic-core-test.journal";
    std::remove(path);
    {
        Runtime runtime(path);
        auto result = runtime.run({"persistent", "core.execute.echo", "persist", true});
        require(result.committed, "persistent action did not commit");
        require(runtime.events().all().size() == 6, "persistent journal event count mismatch");
    }
    {
        Runtime recovered(path);
        require(recovered.events().all().size() == 6, "restart did not recover journal");
        require(recovered.events().verify_chain(), "recovered journal chain invalid");
        auto result = recovered.run({"after restart", "core.execute.echo", "again", true});
        require(result.committed, "post-restart action did not commit");
        require(recovered.events().all().size() == 12, "post-restart event sequence mismatch");
        require(recovered.events().verify_chain(), "post-restart chain invalid");
    }
    std::remove(path);
}

static void test_tampered_journal_is_rejected() {
    const char* path = "exotic-core-tamper.journal";
    std::remove(path);
    {
        Runtime runtime(path);
        runtime.run({"tamper", "core.execute.echo", "safe", true});
    }
    {
        std::ofstream out(path, std::ios::app);
        out << "999\t0\tforged\tattacker\tx\tx\tbad\tbad\n";
    }
    bool rejected = false;
    try { Runtime runtime(path); (void)runtime; } catch (...) { rejected = true; }
    require(rejected, "tampered journal was accepted");
    std::remove(path);
}

int main() {
    try {
        test_known_capability_executes_and_verifies();
        test_unknown_capability_is_denied();
        test_ungranted_actor_is_denied();
        test_budget_denial_blocks_execution();
        test_event_sequences_are_monotonic();
        test_journal_survives_restart();
        test_tampered_journal_is_rejected();
        std::cout << "EXOTIC core v0.3 active tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
