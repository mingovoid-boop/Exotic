#pragma once

#include <iostream>
#include <string>

namespace exotic {
enum class LogLevel { Info, Warning, Error, Debug };
class Logger {
public:
static void Log(LogLevel level, const std::string& message) { std::cout << "[" << ToString(level) << "] " << message << std::endl; }
private:
static const char* ToString(LogLevel level) { switch (level) { case LogLevel::Info: return "INFO"; case LogLevel::Warning: return "WARN"; case LogLevel::Error: return "ERROR"; case LogLevel::Debug: return "DEBUG"; default: return "UNKNOWN"; } }
};
}
