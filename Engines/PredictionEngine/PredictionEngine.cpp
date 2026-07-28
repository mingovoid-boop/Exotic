#include "PredictionEngine.hpp"
#include ^<iostream^>

namespace exotic
{

PredictionEngine::PredictionEngine() {}

void PredictionEngine::Initialize()
{
    initialized = true;
    std::cout ^<^< "PredictionEngine initialized." ^<^< std::endl;
}

void PredictionEngine::Observe(const std::string^& input)
{
    observations.push_back(input);
}

void PredictionEngine::Update()
{
    if (!initialized) return;
}

void PredictionEngine::Shutdown()
{
    initialized = false;
}

}
