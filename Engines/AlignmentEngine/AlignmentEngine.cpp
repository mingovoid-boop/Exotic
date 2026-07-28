#include "AlignmentEngine.hpp"
#include ^<iostream^>

namespace exotic
{

AlignmentEngine::AlignmentEngine() {}

void AlignmentEngine::Initialize()
{
    initialized = true;
    std::cout ^<^< "AlignmentEngine initialized." ^<^< std::endl;
}

void AlignmentEngine::Observe(const std::string^& input)
{
    observations.push_back(input);
}

void AlignmentEngine::Update()
{
    if (!initialized) return;
}

void AlignmentEngine::Shutdown()
{
    initialized = false;
}

}
