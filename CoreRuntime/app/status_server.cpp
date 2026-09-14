#include "exotic/core/status_api.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
constexpr socket_t invalid_socket_value = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
constexpr socket_t invalid_socket_value = -1;
#endif

namespace {

#ifdef _WIN32
class SocketRuntime {
public:
    SocketRuntime() {
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) throw std::runtime_error("WSAStartup failed");
    }
    ~SocketRuntime() { WSACleanup(); }
};
void close_socket(socket_t socket) { closesocket(socket); }
#else
class SocketRuntime {};
void close_socket(socket_t socket) { close(socket); }
#endif

class SocketHandle {
public:
    explicit SocketHandle(socket_t value = invalid_socket_value) : value_(value) {}
    ~SocketHandle() { if (value_ != invalid_socket_value) close_socket(value_); }
    SocketHandle(const SocketHandle&) = delete;
    SocketHandle& operator=(const SocketHandle&) = delete;
    socket_t get() const noexcept { return value_; }
private:
    socket_t value_;
};

const char* reason_phrase(int status) {
    switch (status) {
        case 200: return "OK";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        default: return "Error";
    }
}

bool send_all(socket_t socket, const std::string& data) {
    std::size_t sent = 0;
    while (sent < data.size()) {
#ifdef _WIN32
        const int count = send(socket, data.data() + sent, static_cast<int>(data.size() - sent), 0);
#else
        const auto count = send(socket, data.data() + sent, data.size() - sent, 0);
#endif
        if (count <= 0) return false;
        sent += static_cast<std::size_t>(count);
    }
    return true;
}

std::string make_http_response(const exotic::core::HttpResponse& response) {
    std::ostringstream out;
    out << "HTTP/1.1 " << response.status << ' ' << reason_phrase(response.status) << "\r\n"
        << "Content-Type: " << response.content_type << "\r\n"
        << "Content-Length: " << response.body.size() << "\r\n"
        << "Cache-Control: no-store\r\n"
        << "X-Content-Type-Options: nosniff\r\n"
        << "Connection: close\r\n\r\n"
        << response.body;
    return out.str();
}

struct Options {
    std::uint16_t port{8787};
    std::size_t max_requests{0};
    std::string journal{"exotic-core-v0.2.journal"};
};

Options parse_options(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--port" && i + 1 < argc) {
            const auto value = std::stoul(argv[++i]);
            if (value == 0 || value > 65535) throw std::runtime_error("invalid --port");
            options.port = static_cast<std::uint16_t>(value);
        } else if (arg == "--max-requests" && i + 1 < argc) {
            options.max_requests = static_cast<std::size_t>(std::stoull(argv[++i]));
        } else if (arg == "--journal" && i + 1 < argc) {
            options.journal = argv[++i];
        } else {
            throw std::runtime_error("usage: exotic_core_status [--port N] [--max-requests N] [--journal PATH]");
        }
    }
    return options;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const auto options = parse_options(argc, argv);
        SocketRuntime sockets;
        exotic::core::Runtime runtime(options.journal);
        exotic::core::StatusApi api(runtime);

        SocketHandle server(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
        if (server.get() == invalid_socket_value) throw std::runtime_error("socket creation failed");

        int reuse = 1;
#ifdef _WIN32
        setsockopt(server.get(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));
#else
        setsockopt(server.get(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(options.port);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        if (bind(server.get(), reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0)
            throw std::runtime_error("bind failed");
        if (listen(server.get(), 16) != 0) throw std::runtime_error("listen failed");

        std::cout << "EXOTIC Core status listening on http://127.0.0.1:" << options.port << "\n";
        std::size_t handled = 0;
        while (options.max_requests == 0 || handled < options.max_requests) {
            SocketHandle client(accept(server.get(), nullptr, nullptr));
            if (client.get() == invalid_socket_value) continue;

            char buffer[16384];
#ifdef _WIN32
            const int count = recv(client.get(), buffer, static_cast<int>(sizeof(buffer)), 0);
#else
            const auto count = recv(client.get(), buffer, sizeof(buffer), 0);
#endif
            exotic::core::HttpResponse response{400, "application/json", "{\"error\":\"bad_request\"}"};
            if (count > 0) {
                const std::string request(buffer, static_cast<std::size_t>(count));
                const auto line_end = request.find("\r\n");
                std::istringstream line(request.substr(0, line_end));
                std::string method;
                std::string path;
                std::string version;
                if (line >> method >> path >> version) {
                    if (const auto query = path.find('?'); query != std::string::npos) path.resize(query);
                    response = api.route(method, path);
                }
            }
            send_all(client.get(), make_http_response(response));
            ++handled;
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "EXOTIC Core status server error: " << error.what() << '\n';
        return 1;
    }
}
