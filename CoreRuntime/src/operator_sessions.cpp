#include "exotic/cognition/operator_sessions.hpp"

#include <array>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace exotic::cognition {
namespace {

constexpr std::array<std::uint32_t, 64> K = {
  0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
  0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
  0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
  0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
  0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
  0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
  0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
  0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

std::uint32_t rotr(std::uint32_t x, std::uint32_t n) { return (x >> n) | (x << (32 - n)); }

std::string sha256(const std::string& input) {
  std::vector<std::uint8_t> msg(input.begin(), input.end());
  const std::uint64_t bit_len = static_cast<std::uint64_t>(msg.size()) * 8;
  msg.push_back(0x80);
  while ((msg.size() % 64) != 56) msg.push_back(0);
  for (int i = 7; i >= 0; --i) msg.push_back(static_cast<std::uint8_t>((bit_len >> (i * 8)) & 0xff));

  std::array<std::uint32_t, 8> h = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
  for (std::size_t offset = 0; offset < msg.size(); offset += 64) {
    std::uint32_t w[64]{};
    for (int i = 0; i < 16; ++i) {
      const auto j = offset + static_cast<std::size_t>(i * 4);
      w[i] = (static_cast<std::uint32_t>(msg[j]) << 24) | (static_cast<std::uint32_t>(msg[j+1]) << 16) |
             (static_cast<std::uint32_t>(msg[j+2]) << 8) | static_cast<std::uint32_t>(msg[j+3]);
    }
    for (int i = 16; i < 64; ++i) {
      const auto s0 = rotr(w[i-15],7) ^ rotr(w[i-15],18) ^ (w[i-15] >> 3);
      const auto s1 = rotr(w[i-2],17) ^ rotr(w[i-2],19) ^ (w[i-2] >> 10);
      w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    auto a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
    for (int i = 0; i < 64; ++i) {
      const auto S1 = rotr(e,6) ^ rotr(e,11) ^ rotr(e,25);
      const auto ch = (e & f) ^ ((~e) & g);
      const auto temp1 = hh + S1 + ch + K[i] + w[i];
      const auto S0 = rotr(a,2) ^ rotr(a,13) ^ rotr(a,22);
      const auto maj = (a & b) ^ (a & c) ^ (b & c);
      const auto temp2 = S0 + maj;
      hh=g; g=f; f=e; e=d+temp1; d=c; c=b; b=a; a=temp1+temp2;
    }
    h[0]+=a; h[1]+=b; h[2]+=c; h[3]+=d; h[4]+=e; h[5]+=f; h[6]+=g; h[7]+=hh;
  }
  std::ostringstream out;
  out << std::hex << std::setfill('0');
  for (auto v : h) out << std::setw(8) << v;
  return out.str();
}

}  // namespace

std::string OperatorSessionManager::hash_token(const std::string& raw_token) { return sha256(raw_token); }

OperatorSession OperatorSessionManager::create(const std::string& operator_id,
                                               const std::string& raw_token,
                                               std::int64_t now_unix,
                                               std::int64_t ttl_seconds) {
  if (operator_id.empty() || raw_token.size() < 16 || ttl_seconds <= 0) throw std::invalid_argument("invalid session parameters");
  const auto digest = hash_token(raw_token);
  OperatorSession session{"session-" + digest.substr(0, 16), operator_id, digest, now_unix + ttl_seconds, false};
  store_.create_session(session);
  return session;
}

bool OperatorSessionManager::validate(const std::string& session_id,
                                      const std::string& raw_token,
                                      std::int64_t now_unix) const {
  const auto session = store_.find_session(session_id);
  if (!session || session->revoked || session->expires_unix <= now_unix) return false;
  const auto candidate = hash_token(raw_token);
  if (candidate.size() != session->token_hash.size()) return false;
  unsigned diff = 0;
  for (std::size_t i = 0; i < candidate.size(); ++i) diff |= static_cast<unsigned>(candidate[i] ^ session->token_hash[i]);
  return diff == 0;
}

void OperatorSessionManager::revoke(const std::string& session_id) { store_.revoke_session(session_id); }

}  // namespace exotic::cognition
