#include "exotic/core/auth.hpp"

#include <utility>

namespace exotic::core {

AuthService::AuthService(PasswordHasher hasher, PasswordVerifier verifier, TokenGenerator token_generator)
    : hasher_(std::move(hasher)), verifier_(std::move(verifier)), token_generator_(std::move(token_generator)) {}

bool AuthService::create_account(std::string account_id, std::string username, std::string identity_id,
                                 const std::string& password) {
    if (account_id.empty() || username.empty() || identity_id.empty() || password.empty()) return false;
    if (!hasher_ || usernames_.contains(username) || accounts_.contains(account_id)) return false;
    const std::string hash = hasher_(password);
    if (hash.empty()) return false;
    usernames_.emplace(username, account_id);
    accounts_.emplace(account_id, Account{std::move(account_id), std::move(username), std::move(identity_id), hash});
    return true;
}

AuthResult AuthService::login(const std::string& username, const std::string& password, std::chrono::minutes ttl) {
    const auto username_it = usernames_.find(username);
    if (username_it == usernames_.end()) return {false, "invalid credentials", std::nullopt};
    auto account_it = accounts_.find(username_it->second);
    if (account_it == accounts_.end() || !account_it->second.enabled) return {false, "account disabled or unavailable", std::nullopt};
    auto& account = account_it->second;
    if (!verifier_ || !verifier_(password, account.password_hash)) {
        ++account.failed_attempts;
        return {false, "invalid credentials", std::nullopt};
    }
    if (!token_generator_) return {false, "secure token generator unavailable", std::nullopt};
    const std::string token = token_generator_();
    if (token.empty() || sessions_.contains(token)) return {false, "session token generation failed", std::nullopt};
    account.failed_attempts = 0;
    Session session{token, account.id, account.identity_id, std::chrono::system_clock::now() + ttl, false};
    sessions_.emplace(token, session);
    return {true, "authenticated", session};
}

bool AuthService::logout(const std::string& token) {
    auto it = sessions_.find(token);
    if (it == sessions_.end()) return false;
    it->second.revoked = true;
    return true;
}

const Session* AuthService::validate(const std::string& token) const {
    const auto it = sessions_.find(token);
    if (it == sessions_.end() || it->second.revoked || it->second.expires_at <= std::chrono::system_clock::now()) return nullptr;
    const auto account_it = accounts_.find(it->second.account_id);
    if (account_it == accounts_.end() || !account_it->second.enabled) return nullptr;
    return &it->second;
}

bool AuthService::disable_account(const std::string& account_id) {
    auto it = accounts_.find(account_id);
    if (it == accounts_.end()) return false;
    it->second.enabled = false;
    for (auto& [token, session] : sessions_) if (session.account_id == account_id) session.revoked = true;
    return true;
}

} // namespace exotic::core
