#pragma once

#include <atomic>
#include <cstdint>

namespace exotic {
using UUID = std::uint64_t;
class UUIDGenerator {
public:
static UUID Next() { static std::atomic<UUID> counter{1}; return counter++; }
};
}

