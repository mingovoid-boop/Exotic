#include "ManufacturingDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void ManufacturingDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Manufacturing domain initialized." ^<^< std::endl;
}

void ManufacturingDomain::RegisterModules()
{
    if (!active) return;
}

void ManufacturingDomain::Shutdown()
{
    active = false;
}

}
