#include "exotic/core/runtime.hpp"
#include "exotic/core/crypto.hpp"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace exotic::core;

static void test_sha256_known_answers() {
    assert(crypto::sha256_hex("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    assert(crypto::sha256_hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

static void test_known_capability_executes_and_verifies() {
    Runtime runtime;
    auto result = runtime.run({"echo test", "core.execute.echo", "hello", true});
    assert(result.executed && result.verification.passed && result.committed);
    assert(result.output == "hello");
    assert(runtime.events().all().size() == 5);
    assert(runtime.events().verify_chain());
    for (const auto& event : runtime.events().all()) assert(event.hash.size() == 64);
}

static void test_unknown_capability_is_denied() {
    Runtime runtime;
    auto result = runtime.run({"deny test", "missing.capability", "x", true});
    assert(!result.executed && !result.committed && !result.verification.passed);
    assert(runtime.events().all().size() == 2);
}

static void test_ungranted_actor_is_denied() {
    Runtime runtime;
    runtime.identities().register_identity({"guest", true, {"core.observe"}});
    auto result = runtime.run({"unauthorized", "core.execute.echo", "x", true, "guest"});
    assert(!result.executed);
    assert(!result.committed);
    assert(result.verification.evidence == "actor lacks capability grant");
}

static void test_event_sequences_are_monotonic() {
    Runtime runtime;
    runtime.run({"one", "core.execute.echo", "1", true});
    runtime.run({"two", "core.execute.echo", "2", true});
    const auto& events = runtime.events().all();
    for (std::size_t i=0; i<events.size(); ++i) assert(events[i].sequence == i+1);
    assert(runtime.events().verify_chain());
}

static void test_journal_survives_restart() {
    const char* path = "exotic-core-test.journal";
    std::remove(path);
    {
        Runtime runtime(path);
        auto result=runtime.run({"persistent", "core.execute.echo", "persist", true});
        assert(result.committed);
        assert(runtime.events().all().size()==5);
        for (const auto& event : runtime.events().all()) assert(event.hash.size() == 64);
    }
    {
        Runtime recovered(path);
        assert(recovered.events().all().size()==5);
        assert(recovered.events().verify_chain());
        auto result=recovered.run({"after restart", "core.execute.echo", "again", true});
        assert(result.committed);
        assert(recovered.events().all().size()==10);
        assert(recovered.events().verify_chain());
    }
    std::remove(path);
}

static void test_tampered_journal_is_rejected() {
    const char* path="exotic-core-tamper.journal";
    std::remove(path);
    {
        Runtime runtime(path);
        runtime.run({"tamper", "core.execute.echo", "safe", true});
    }
    {
        std::ofstream out(path, std::ios::app);
        out << "999\t0\tforged\tattacker\tx\tx\tbad\tbad\n";
    }
    bool rejected=false;
    try { Runtime runtime(path); (void)runtime; } catch (...) { rejected=true; }
    assert(rejected);
    std::remove(path);
}

int main() {
    test_sha256_known_answers();
    test_known_capability_executes_and_verifies();
    test_unknown_capability_is_denied();
    test_ungranted_actor_is_denied();
    test_event_sequences_are_monotonic();
    test_journal_survives_restart();
    test_tampered_journal_is_rejected();
    std::cout << "EXOTIC core v0.2 tests passed\n";
    return 0;
}
