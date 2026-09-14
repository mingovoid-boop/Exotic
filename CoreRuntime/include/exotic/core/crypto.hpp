#pragma once

#include <string>
#include <string_view>

namespace exotic::core::crypto {

// Returns the lowercase hexadecimal SHA-256 digest of input.
// Self-contained so the Core evidence chain has identical semantics on
// supported Windows and Linux builds without an external crypto dependency.
std::string sha256_hex(std::string_view input);

} // namespace exotic::core::crypto
