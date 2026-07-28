#include "CLI.hpp"
#include "../Generators/ClassGenerator.hpp"
#include "../Generators/ModuleGenerator.hpp"
#include "../Generators/EngineGenerator.hpp"
#include "../Generators/DomainGenerator.hpp"
#include "../Generators/CoreGenerator.hpp"
#include "../Generators/DomainPackGenerator.hpp"
#include "../Generators/BuildAllGenerator.hpp"
#include "../Commands/StatusCommand.hpp"
#include <iostream>
#include <string>
namespace exotic { int CLI::Run(int argc,char** argv){ if(argc < 2){ std::cout << "Usage: exotic status ^| build-core ^| build-domains ^| build-all ^| create-class Name ^| create-module Name ^| create-engine Name ^| create-domain Name" << std::endl; return 0; } std::string command=argv[1]; if(command=="status"){ StatusCommand::Run(); return 0; } if(command=="build-core"){ CoreGenerator::Generate(); std::cout << "Generated Exotic Core." << std::endl; return 0; } if(command=="build-domains"){ DomainPackGenerator::Generate(); std::cout << "Generated Exotic Domains." << std::endl; return 0; } if(command=="build-all"){ BuildAllGenerator::Generate(); std::cout << "Generated Exotic Core + Domains + Project Map." << std::endl; return 0; } if(argc < 3){ std::cout << "Missing name." << std::endl; return 1; } std::string name=argv[2]; if(command=="create-class"){ ClassGenerator::Generate(name); std::cout << "Generated class: " << name << std::endl; return 0; } if(command=="create-module"){ ModuleGenerator::Generate(name); std::cout << "Generated module: " << name << std::endl; return 0; } if(command=="create-engine"){ EngineGenerator::Generate(name); std::cout << "Generated engine: " << name << "Engine" << std::endl; return 0; } if(command=="create-domain"){ DomainGenerator::Generate(name); std::cout << "Generated domain: " << name << std::endl; return 0; } std::cout << "Unknown command." << std::endl; return 1; } }
