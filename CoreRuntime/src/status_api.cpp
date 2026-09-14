#include "exotic/core/status_api.hpp"

#include <chrono>
#include <sstream>

namespace exotic::core {
namespace {

std::string json_escape(std::string_view value) {
    std::string out;
    out.reserve(value.size() + 8);
    for (const unsigned char c : value) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    static constexpr char hex[] = "0123456789abcdef";
                    out += "\\u00";
                    out += hex[(c >> 4) & 0x0f];
                    out += hex[c & 0x0f];
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

const char* authority_name(Authority authority) {
    switch (authority) {
        case Authority::none: return "none";
        case Authority::read: return "read";
        case Authority::sandbox: return "sandbox";
        case Authority::external: return "external";
    }
    return "unknown";
}

const char* risk_name(Risk risk) {
    switch (risk) {
        case Risk::low: return "low";
        case Risk::medium: return "medium";
        case Risk::high: return "high";
    }
    return "unknown";
}

std::int64_t epoch_millis(std::chrono::system_clock::time_point timestamp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
}

HttpResponse json_error(int status, std::string_view code) {
    return {status, "application/json", "{\"error\":\"" + json_escape(code) + "\"}"};
}

} // namespace

HttpResponse StatusApi::route(std::string_view method, std::string_view path) const {
    if (method != "GET") return json_error(405, "method_not_allowed");

    const auto integrity = runtime_.events().verify_chain();
    const auto& events = runtime_.events().all();
    const auto capabilities = runtime_.capabilities().snapshot();

    if (path == "/health") {
        std::ostringstream out;
        out << "{\"platform\":\"EXOTIC\",\"status\":\"" << (integrity ? "ready" : "degraded")
            << "\",\"integrity\":" << (integrity ? "true" : "false")
            << ",\"event_count\":" << events.size()
            << ",\"capability_count\":" << capabilities.size()
            << ",\"journal_persistence_configured\":" << (runtime_.events().durable() ? "true" : "false")
            << "}";
        return {200, "application/json", out.str()};
    }

    if (path == "/version") {
        return {200, "application/json",
                "{\"platform\":\"EXOTIC\",\"version\":\"" + std::string(kPlatformVersion) +
                    "\",\"status\":\"prototype\"}"};
    }

    if (path == "/capabilities") {
        std::ostringstream out;
        out << "{\"capabilities\":[";
        for (std::size_t i = 0; i < capabilities.size(); ++i) {
            const auto& capability = capabilities[i];
            if (i) out << ',';
            out << "{\"id\":\"" << json_escape(capability.id)
                << "\",\"authority\":\"" << authority_name(capability.authority)
                << "\",\"risk\":\"" << risk_name(capability.risk)
                << "\",\"enabled\":" << (capability.enabled ? "true" : "false") << '}';
        }
        out << "]}";
        return {200, "application/json", out.str()};
    }

    if (path == "/events") {
        std::ostringstream out;
        out << "{\"events\":[";
        for (std::size_t i = 0; i < events.size(); ++i) {
            const auto& event = events[i];
            if (i) out << ',';
            out << "{\"sequence\":" << event.sequence
                << ",\"type\":\"" << json_escape(event.type)
                << "\",\"actor\":\"" << json_escape(event.actor)
                << "\",\"subject\":\"" << json_escape(event.subject)
                << "\",\"payload\":\"" << json_escape(event.payload)
                << "\",\"timestamp_ms\":" << epoch_millis(event.timestamp)
                << ",\"previous_hash\":\"" << json_escape(event.previous_hash)
                << "\",\"hash\":\"" << json_escape(event.hash) << "\"}";
        }
        out << "],\"integrity\":" << (integrity ? "true" : "false") << '}';
        return {200, "application/json", out.str()};
    }

    if (path == "/public/status") {
        const auto last_sequence = events.empty() ? 0 : events.back().sequence;
        const auto last_event_time = events.empty() ? 0 : epoch_millis(events.back().timestamp);
        std::ostringstream out;
        out << "{\"platform\":\"EXOTIC\",\"version\":\"" << kPlatformVersion
            << "\",\"runtime\":\"" << (integrity ? "ready" : "degraded")
            << "\",\"integrity\":" << (integrity ? "true" : "false")
            << ",\"event_count\":" << events.size()
            << ",\"capability_count\":" << capabilities.size()
            << ",\"last_sequence\":" << last_sequence
            << ",\"last_event_time_ms\":" << last_event_time << '}';
        return {200, "application/json", out.str()};
    }

    return json_error(404, "not_found");
}

} // namespace exotic::core
