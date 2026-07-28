#include "PerceptionEngine.hpp"
#include ^<iostream^>

namespace exotic
{

PerceptionEngine::PerceptionEngine() {}

void PerceptionEngine::Initialize()
{
    initialized = true;
    std::cout ^<^< "PerceptionEngine initialized from XIR." ^<^< std::endl;
}

void PerceptionEngine::Observe(const std::string^& input)
{
    observations.push_back(input);
}

void PerceptionEngine::Update()
{
    if (!initialized) return;
}

void PerceptionEngine::Shutdown()
{
    initialized = false;
}

}
