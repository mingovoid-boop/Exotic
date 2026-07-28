#include "HealthcareDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void HealthcareDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Healthcare domain initialized." ^<^< std::endl;
}

void HealthcareDomain::RegisterModules()
{
    if (!active) return;
}

void HealthcareDomain::Shutdown()
{
    active = false;
}

}
