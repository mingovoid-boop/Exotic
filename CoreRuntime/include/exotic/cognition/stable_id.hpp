#pragma once
#include <atomic>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
namespace exotic::cognition {
inline std::string stable_id(const std::string& prefix){
  static std::atomic<std::uint64_t> counter{0};
  thread_local std::mt19937_64 rng{std::random_device{}()};
  const auto ticks=static_cast<std::uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
  const auto seq=counter.fetch_add(1,std::memory_order_relaxed);
  const auto rnd=rng();
  std::ostringstream out;out<<prefix<<'-'<<std::hex<<ticks<<'-'<<seq<<'-'<<rnd;return out.str();
}
} // namespace exotic::cognition
