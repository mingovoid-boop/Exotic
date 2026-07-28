#include "ObserverEngine.hpp"
#include ^<iostream^>

namespace exotic
{

ObserverEngine::ObserverEngine() {}

void ObserverEngine::Initialize()
{
    initialized = true;
    std::cout ^<^< "ObserverEngine initialized." ^<^< std::endl;
}

void ObserverEngine::Observe(const std::string^& input)
{
    observations.push_back(input);
}

void ObserverEngine::Update()
{
    if (!initialized) return;
}

void ObserverEngine::Shutdown()
{
    initialized = false;
}

}
