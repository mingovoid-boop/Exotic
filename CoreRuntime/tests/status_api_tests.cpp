#include "exotic/core/status_api.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace exotic::core;

static bool contains(const std::string& value, const std::string& needle) {
    return value.find(needle) != std::string::npos;
}

static void test_internal_status_routes() {
    Runtime runtime;
    const auto result = runtime.run({"status test", "core.execute.echo", "hello\n\"world\"", true});
    assert(result.committed);

    StatusApi api(runtime);

    const auto health = api.route("GET", "/health");
    assert(health.status == 200);
    assert(contains(health.body, "\"platform\":\"EXOTIC\""));
    assert(contains(health.body, "\"integrity\":true"));
    assert(contains(health.body, "\"event_count\":5"));
    assert(contains(health.body, "\"capability_count\":3"));

    const auto version = api.route("GET", "/version");
    assert(version.status == 200);
    assert(contains(version.body, "\"version\":\"0.2.0-dev\""));

    const auto capabilities = api.route("GET", "/capabilities");
    assert(capabilities.status == 200);
    const auto execute = capabilities.body.find("core.execute.echo");
    const auto observe = capabilities.body.find("core.observe");
    const auto plan = capabilities.body.find("core.plan");
    assert(execute < observe && observe < plan);

    const auto events = api.route("GET", "/events");
    assert(events.status == 200);
    assert(contains(events.body, "\"actor\":"));
    assert(contains(events.body, "\"payload\":"));
    assert(contains(events.body, "\"previous_hash\":"));
    assert(contains(events.body, "hello\\n\\\"world\\\""));
}

static void test_public_status_is_sanitized() {
    Runtime runtime;
    runtime.run({"public test", "core.execute.echo", "sensitive payload", true});
    StatusApi api(runtime);

    const auto response = api.route("GET", "/public/status");
    assert(response.status == 200);
    assert(contains(response.body, "\"platform\":\"EXOTIC\""));
    assert(contains(response.body, "\"version\":\"0.2.0-dev\""));
    assert(contains(response.body, "\"runtime\":\"ready\""));
    assert(contains(response.body, "\"integrity\":true"));
    assert(contains(response.body, "\"event_count\":5"));
    assert(contains(response.body, "\"capability_count\":3"));
    assert(contains(response.body, "\"last_sequence\":5"));

    // Public projection allowlist: no raw evidence identity, content, or cryptographic material.
    assert(!contains(response.body, "actor"));
    assert(!contains(response.body, "subject"));
    assert(!contains(response.body, "payload"));
    assert(!contains(response.body, "previous_hash"));
    assert(!contains(response.body, "\"hash\""));
    assert(!contains(response.body, "sensitive payload"));
}

static void test_method_and_unknown_route_denials() {
    Runtime runtime;
    StatusApi api(runtime);
    assert(api.route("POST", "/health").status == 405);
    assert(api.route("GET", "/missing").status == 404);
}

int main() {
    test_internal_status_routes();
    test_public_status_is_sanitized();
    test_method_and_unknown_route_denials();
    std::cout << "EXOTIC Core v0.2 status API tests passed\n";
    return 0;
}
