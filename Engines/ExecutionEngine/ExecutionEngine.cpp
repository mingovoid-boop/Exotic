#include "ExecutionEngine.hpp"
#include ^<iostream^>

namespace exotic
{

ExecutionEngine::ExecutionEngine() {}

void ExecutionEngine::Initialize()
{
    initialized = true;
    std::cout ^<^< "ExecutionEngine initialized." ^<^< std::endl;
}

void ExecutionEngine::Observe(const std::string^& input)
{
    observations.push_back(input);
}

void ExecutionEngine::Update()
{
    if (!initialized) return;
}

void ExecutionEngine::Shutdown()
{
    initialized = false;
}

}
