cmake_minimum_required(VERSION 3.16)
project(ExoticBuilder)
set(CMAKE_CXX_STANDARD 17)
add_executable(exotic Builder/main.cpp Builder/CLI/CLI.cpp Builder/Core/TemplateEngine.cpp Builder/Generators/ClassGenerator.cpp Builder/Generators/ModuleGenerator.cpp Builder/Generators/EngineGenerator.cpp Builder/Generators/DomainGenerator.cpp Builder/Generators/CoreGenerator.cpp Builder/Generators/DomainPackGenerator.cpp Builder/Generators/BuildAllGenerator.cpp Builder/Utils/FileWriter.cpp Builder/Commands/StatusCommand.cpp)
