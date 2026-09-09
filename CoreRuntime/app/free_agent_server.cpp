#include "exotic/cognition/free_agent_service.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socket_t = SOCKET;
constexpr socket_t invalid_socket = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
constexpr socket_t invalid_socket = -1;
#endif

namespace {

void close_socket(socket_t sock) {
#ifdef _WIN32
  closesocket(sock);
#else
  close(sock);
#endif
}

std::string json_escape(const std::string& input) {
  std::ostringstream out;
  for (const char ch : input) {
    switch (ch) {
      case '\\': out << "\\\\"; break;
      case '"': out << "\\\""; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default: out << ch; break;
    }
  }
  return out.str();
}

std::string extract_json_string(const std::string& body, const std::string& key) {
  const std::string needle = "\"" + key + "\"";
  auto pos = body.find(needle);
  if (pos == std::string::npos) return {};
  pos = body.find(':', pos + needle.size());
  if (pos == std::string::npos) return {};
  pos = body.find('"', pos + 1);
  if (pos == std::string::npos) return {};
  ++pos;
  std::string value;
  bool escaped = false;
  for (; pos < body.size(); ++pos) {
    const char ch = body[pos];
    if (escaped) {
      switch (ch) {
        case 'n': value.push_back('\n'); break;
        case 'r': value.push_back('\r'); break;
        case 't': value.push_back('\t'); break;
        default: value.push_back(ch); break;
      }
      escaped = false;
      continue;
    }
    if (ch == '\\') { escaped = true; continue; }
    if (ch == '"') break;
    value.push_back(ch);
  }
  return value;
}

std::string header_value(const std::string& request, const std::string& name) {
  const std::string needle = name + ":";
  auto pos = request.find(needle);
  if (pos == std::string::npos) return {};
  pos += needle.size();
  while (pos < request.size() && (request[pos] == ' ' || request[pos] == '\t')) ++pos;
  const auto end = request.find("\r\n", pos);
  return request.substr(pos, end == std::string::npos ? std::string::npos : end - pos);
}

bool mutation_authorized(const std::string& request, const std::string& operator_token) {
  if (operator_token.empty()) return true;
  return header_value(request, "Authorization") == "Bearer " + operator_token;
}

std::string thought_json(const exotic::cognition::ServiceThought& t) {
  std::ostringstream out;
  out << "{\"id\":\"" << json_escape(t.id) << "\","
      << "\"subject\":\"" << json_escape(t.subject) << "\","
      << "\"score\":" << t.score << ','
      << "\"reason\":\"" << json_escape(t.reason) << "\","
      << "\"status\":\"" << json_escape(t.status) << "\","
      << "\"createdAt\":\"" << json_escape(t.created_at) << "\"}";
  return out.str();
}

std::string response(int status, const std::string& body) {
  const char* text = status == 200 ? "OK" :
                     status == 201 ? "Created" :
                     status == 204 ? "No Content" :
                     status == 400 ? "Bad Request" :
                     status == 401 ? "Unauthorized" :
                     status == 404 ? "Not Found" : "Internal Server Error";
  std::ostringstream out;
  out << "HTTP/1.1 " << status << ' ' << text << "\r\n"
      << "Content-Type: application/json; charset=utf-8\r\n"
      << "Access-Control-Allow-Origin: http://localhost:4173\r\n"
      << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
      << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
      << "Cache-Control: no-store\r\n"
      << "X-Content-Type-Options: nosniff\r\n"
      << "X-Frame-Options: DENY\r\n"
      << "Content-Security-Policy: default-src 'none'\r\n"
      << "Content-Length: " << body.size() << "\r\n"
      << "Connection: close\r\n\r\n"
      << body;
  return out.str();
}

}  // namespace

int main(int argc, char** argv) {
  int port = 8421;
  if (argc > 1) port = std::stoi(argv[1]);

  const char* token_env = std::getenv("EXOTIC_OPERATOR_TOKEN");
  const std::string operator_token = token_env ? token_env : "";

#ifdef _WIN32
  WSADATA data{};
  if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
    std::cerr << "WSAStartup failed\n";
    return 1;
  }
#endif

  socket_t server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server == invalid_socket) {
    std::cerr << "socket creation failed\n";
    return 1;
  }

  int reuse = 1;
#ifdef _WIN32
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));
#else
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(static_cast<unsigned short>(port));

  if (bind(server, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0 || listen(server, 16) != 0) {
    std::cerr << "failed to bind/listen on 127.0.0.1:" << port << '\n';
    close_socket(server);
    return 1;
  }

  exotic::cognition::FreeAgentService service;
  std::cout << "EXOTIC Free-Agent API listening on http://127.0.0.1:" << port << '\n';
  if (operator_token.empty()) {
    std::cout << "WARNING: EXOTIC_OPERATOR_TOKEN is unset; local mutation endpoints are unauthenticated.\n";
  }

  for (;;) {
    socket_t client = accept(server, nullptr, nullptr);
    if (client == invalid_socket) continue;

    std::string request;
    char buffer[4096];
    for (;;) {
#ifdef _WIN32
      const int n = recv(client, buffer, static_cast<int>(sizeof(buffer)), 0);
#else
      const int n = static_cast<int>(recv(client, buffer, sizeof(buffer), 0));
#endif
      if (n <= 0) break;
      request.append(buffer, static_cast<std::size_t>(n));
      if (request.size() > 1024 * 1024) break;

      const auto header_end = request.find("\r\n\r\n");
      if (header_end != std::string::npos) {
        std::size_t content_length = 0;
        const auto cl = request.find("Content-Length:");
        if (cl != std::string::npos) {
          const auto end = request.find("\r\n", cl);
          content_length = static_cast<std::size_t>(std::stoul(request.substr(cl + 15, end - (cl + 15))));
        }
        if (request.size() >= header_end + 4 + content_length) break;
      }
    }

    const auto line_end = request.find("\r\n");
    const std::string first_line = line_end == std::string::npos ? request : request.substr(0, line_end);
    std::istringstream line(first_line);
    std::string method, path, protocol;
    line >> method >> path >> protocol;
    const auto header_end = request.find("\r\n\r\n");
    const std::string body = header_end == std::string::npos ? std::string{} : request.substr(header_end + 4);

    std::string payload;
    int status = 200;
    try {
      if (method == "OPTIONS") {
        status = 204;
        payload.clear();
      } else if (method == "GET" && path == "/health") {
        payload = service.health_json();
      } else if (method == "GET" && path == "/version") {
        payload = service.version_json();
      } else if (method == "GET" && path == "/api/free-agent/state") {
        payload = service.state_json();
      } else if (method == "POST" && !mutation_authorized(request, operator_token)) {
        status = 401;
        payload = "{\"error\":\"operator authorization required\"}";
      } else if (method == "POST" && path == "/api/free-agent/thoughts") {
        const auto subject = extract_json_string(body, "subject");
        if (subject.empty() || subject.size() > 4096) {
          status = 400;
          payload = "{\"error\":\"subject must contain 1-4096 characters\"}";
        } else {
          payload = thought_json(service.submit_thought(subject));
          status = 201;
        }
      } else if (method == "POST" && path == "/api/free-agent/mode") {
        const auto requested_mode = extract_json_string(body, "mode");
        if (!service.set_mode(requested_mode)) {
          status = 400;
          payload = "{\"error\":\"invalid mode\"}";
        } else {
          payload = service.state_json();
        }
      } else {
        status = 404;
        payload = "{\"error\":\"not found\"}";
      }
    } catch (const std::exception&) {
      status = 500;
      payload = "{\"error\":\"internal server error\"}";
    }

    const auto wire = response(status, payload);
#ifdef _WIN32
    send(client, wire.data(), static_cast<int>(wire.size()), 0);
#else
    send(client, wire.data(), wire.size(), 0);
#endif
    close_socket(client);
  }

  close_socket(server);
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
