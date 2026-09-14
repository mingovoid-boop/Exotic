#pragma once

#include "exotic/core/runtime.hpp"

#include <string>
#include <string_view>

namespace exotic::core {

inline constexpr std::string_view kPlatformVersion = "0.2.0-dev";

struct HttpResponse {
    int status{200};
    std::string content_type{"application/json"};
    std::string body;
};

class StatusApi {
public:
    explicit StatusApi(const Runtime& runtime) : runtime_(runtime) {}
    HttpResponse route(std::string_view method, std::string_view path) const;

private:
    const Runtime& runtime_;
};

} // namespace exotic::core
