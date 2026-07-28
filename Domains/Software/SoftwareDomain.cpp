#include "SoftwareDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void SoftwareDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Software domain initialized." ^<^< std::endl;
}

void SoftwareDomain::RegisterModules()
{
    if (!active) return;
}

void SoftwareDomain::Shutdown()
{
    active = false;
}

}
