#pragma once

#include <string>

namespace exotic {

enum class ErrorCode {
    None,
    Unknown,
    InvalidArgument,
    NotFound,
    AlreadyExists,
    Failed,
    NotImplemented
};

struct Error {
    ErrorCode code = ErrorCode::None;
    std::string message;

    bool Ok() const { return code == ErrorCode::None; }
};

template <typename T>
struct Result {
    T value{};
    Error error{};

    bool Ok() const { return error.Ok(); }
};

}

