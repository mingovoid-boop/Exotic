#pragma once
#include <string>
#include <vector>
namespace exotic { struct XirNode { std::string id; std::string name; std::string type; std::string purpose; std::vector<std::string> capabilities; std::vector<std::string> states; std::vector<std::string> events; std::vector<std::string> dependencies; }; }
