#include "FileWriter.hpp"
#include <fstream>
#include <filesystem>
namespace exotic { bool FileWriter::Write(const std::string& path,const std::string& content){ std::filesystem::create_directories(std::filesystem::path(path).parent_path()); std::ofstream file(path); if(!file) return false; file << content; return true; } }
