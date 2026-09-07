#include "exotic/core/runtime.hpp"

#include <cassert>
#include <iostream>

using namespace exotic::core;

static void test_known_capability_executes_and_verifies() {
    Runtime runtime;
    auto result = runtime.run({"echo test", "core.execute.echo", "hello", true});
    assert(result.executed);
    assert(result.verification.passed);
    assert(result.committed);
    assert(result.output == "hello");
    assert(runtime.events().all().size() == 5);
}

static void test_unknown_capability_is_denied() {
    Runtime runtime;
    auto result = runtime.run({"deny test", "missing.capability", "x", true});
    assert(!result.executed);
    assert(!result.committed);
    assert(!result.verification.passed);
    assert(runtime.events().all().size() == 2);
}

static void test_event_sequences_are_monotonic() {
    Runtime runtime;
    runtime.run({"one", "core.execute.echo", "1", true});
    runtime.run({"two", "core.execute.echo", "2", true});
    const auto& events = runtime.events().all();
    for (std::size_t i = 0; i < events.size(); ++i) assert(events[i].sequence == i + 1);
}

int main() {
    test_known_capability_executes_and_verifies();
    test_unknown_capability_is_denied();
    test_event_sequences_are_monotonic();
    std::cout << "EXOTIC core tests passed\n";
    return 0;
}
