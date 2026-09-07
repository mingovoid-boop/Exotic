#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace exotic::core {

using PasswordHasher = std::function<std::string(const std::string&)>;
using PasswordVerifier = std::function<bool(const std::string&, const std::string&)>;
using TokenGenerator = std::function<std::string()>;

struct Account {
    std::string id;
    std::string username;
    std::string identity_id;
    std::string password_hash;
    bool enabled{true};
    std::uint32_t failed_attempts{0};
};

struct Session {
    std::string token;
    std::string account_id;
    std::string identity_id;
    std::chrono::system_clock::time_point expires_at;
    bool revoked{false};
};

struct AuthResult {
    bool authenticated{false};
    std::string reason;
    std::optional<Session> session;
};

class AuthService {
public:
    AuthService(PasswordHasher hasher, PasswordVerifier verifier, TokenGenerator token_generator);
    bool create_account(std::string account_id, std::string username, std::string identity_id,
                        const std::string& password);
    AuthResult login(const std::string& username, const std::string& password,
                     std::chrono::minutes ttl = std::chrono::minutes(60));
    bool logout(const std::string& token);
    const Session* validate(const std::string& token) const;
    bool disable_account(const std::string& account_id);

private:
    PasswordHasher hasher_;
    PasswordVerifier verifier_;
    TokenGenerator token_generator_;
    std::unordered_map<std::string, Account> accounts_;
    std::unordered_map<std::string, std::string> usernames_;
    std::unordered_map<std::string, Session> sessions_;
};

} // namespace exotic::core
