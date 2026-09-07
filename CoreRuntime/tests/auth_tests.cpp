#include "exotic/core/auth.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

using namespace exotic::core;

static void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error("TEST FAILURE: " + message);
}

int main() {
    try {
        int token_counter = 0;
        AuthService auth(
            [](const std::string& password) { return "TEST$" + password; },
            [](const std::string& password, const std::string& hash) { return hash == "TEST$" + password; },
            [&token_counter]() { return "token-" + std::to_string(++token_counter); });

        require(auth.create_account("acct-1", "alice", "identity.alice", "correct-horse"),
                "account creation failed");
        require(!auth.create_account("acct-2", "alice", "identity.other", "pw"),
                "duplicate username accepted");

        auto bad = auth.login("alice", "wrong");
        require(!bad.authenticated, "wrong password authenticated");
        require(!bad.session.has_value(), "failed login returned session");

        auto good = auth.login("alice", "correct-horse");
        require(good.authenticated, "valid credentials rejected");
        require(good.session.has_value(), "successful login returned no session");
        const auto token = good.session->token;
        require(auth.validate(token) != nullptr, "fresh session invalid");
        require(auth.validate(token)->identity_id == "identity.alice", "session identity mismatch");

        require(auth.logout(token), "logout failed");
        require(auth.validate(token) == nullptr, "revoked session still valid");

        auto second = auth.login("alice", "correct-horse");
        require(second.authenticated && second.session.has_value(), "second login failed");
        require(auth.disable_account("acct-1"), "account disable failed");
        require(auth.validate(second.session->token) == nullptr, "disabled account session still valid");
        require(!auth.login("alice", "correct-horse").authenticated, "disabled account authenticated");

        std::cout << "EXOTIC auth tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
