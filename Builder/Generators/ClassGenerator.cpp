#include "ClassGenerator.hpp"
#include "../Core/TemplateEngine.hpp"
#include "../Utils/FileWriter.hpp"
namespace exotic { void ClassGenerator::Generate(const std::string& name){ FileWriter::Write("include/"+name+".hpp",TemplateEngine::Header(name)); FileWriter::Write("src/"+name+".cpp",TemplateEngine::Source(name)); } }
