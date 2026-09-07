#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>

namespace exotic::core {

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
    bool create_account(std::string account_id, std::string username, std::string identity_id,
                        const std::string& password);
    AuthResult login(const std::string& username, const std::string& password,
                     std::chrono::minutes ttl = std::chrono::minutes(60));
    bool logout(const std::string& token);
    const Session* validate(const std::string& token) const;
    bool disable_account(const std::string& account_id);

private:
    static std::string hash_password(const std::string& password, const std::string& salt);
    static std::string make_token();
    std::unordered_map<std::string, Account> accounts_;
    std::unordered_map<std::string, std::string> usernames_;
    std::unordered_map<std::string, Session> sessions_;
};

} // namespace exotic::core
