#include "BuildAllGenerator.hpp"
#include "CoreGenerator.hpp"
#include "DomainPackGenerator.hpp"
#include "../Utils/FileWriter.hpp"
#include <filesystem>
namespace exotic { void BuildAllGenerator::Generate(){ std::filesystem::create_directories("ExoticGenerated"); CoreGenerator::Generate(); DomainPackGenerator::Generate(); FileWriter::Write("ExoticGenerated/PROJECT_MAP.md","# EXOTIC GENERATED PROJECT MAP\n\n## Core\n\n- MemoryEngine\n- ObserverEngine\n- ExecutionEngine\n- PredictionEngine\n- AlignmentEngine\n\n## Domains\n\n- Business\n- Software\n- Games\n- AI\n- Robotics\n- Creative\n- Science\n- Education\n- Personal\n- Healthcare\n- Manufacturing\n- Finance\n- Security\n- Data\n- IoT\n\nCore is invisible infrastructure. Domains are customer-facing expressions of Core.\n"); } }
