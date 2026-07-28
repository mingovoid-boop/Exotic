#include "MemoryEngine.hpp"
#include ^<iostream^>

namespace exotic
{

MemoryEngine::MemoryEngine() {}

void MemoryEngine::Initialize()
{
    initialized = true;
    std::cout ^<^< "MemoryEngine initialized." ^<^< std::endl;
}

void MemoryEngine::Observe(const std::string^& input)
{
    observations.push_back(input);
}

void MemoryEngine::Update()
{
    if (!initialized) return;
}

void MemoryEngine::Shutdown()
{
    initialized = false;
}

}
