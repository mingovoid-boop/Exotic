cmake_minimum_required(VERSION 3.20)
project(EXOTIC LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CTest at the top level so tests registered by subdirectories are
# visible when CI runs `ctest --test-dir build`.
include(CTest)
enable_testing()

option(EXOTIC_BUILD_LEGACY_BUILDER "Build the legacy EXOTIC generator CLI" ON)
option(EXOTIC_BUILD_CORE_RUNTIME "Build EXOTIC Core v0.1 runtime" ON)

if(EXOTIC_BUILD_LEGACY_BUILDER)
  add_executable(exotic
    Builder/main.cpp
    Builder/CLI/CLI.cpp
    Builder/Core/TemplateEngine.cpp
    Builder/Generators/ClassGenerator.cpp
    Builder/Generators/ModuleGenerator.cpp
    Builder/Generators/EngineGenerator.cpp
    Builder/Generators/DomainGenerator.cpp
    Builder/Generators/CoreGenerator.cpp
    Builder/Generators/DomainPackGenerator.cpp
    Builder/Generators/BuildAllGenerator.cpp
    Builder/Utils/FileWriter.cpp
    Builder/Commands/StatusCommand.cpp
  )
endif()

if(EXOTIC_BUILD_CORE_RUNTIME)
  add_subdirectory(CoreRuntime)
endif()
