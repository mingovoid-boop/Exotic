#include "AIDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void AIDomain::Initialize()
{
    active = true;
    std::cout ^<^< "AI domain initialized." ^<^< std::endl;
}

void AIDomain::RegisterModules()
{
    if (!active) return;
}

void AIDomain::Shutdown()
{
    active = false;
}

}
