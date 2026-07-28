#include "StatusCommand.hpp"
#include <iostream>
#include <filesystem>
namespace exotic { void StatusCommand::Run(){ std::cout << "EXOTIC PROJECT STATUS" << std::endl; std::cout << "Core: " << (std::filesystem::exists("Core") ? "YES" : "NO") << std::endl; std::cout << "Engines: " << (std::filesystem::exists("Engines") ? "YES" : "NO") << std::endl; std::cout << "Domains: " << (std::filesystem::exists("Domains") ? "YES" : "NO") << std::endl; std::cout << "Modules: " << (std::filesystem::exists("Modules") ? "YES" : "NO") << std::endl; std::cout << "Generated Map: " << (std::filesystem::exists("ExoticGenerated/PROJECT_MAP.md") ? "YES" : "NO") << std::endl; } }
