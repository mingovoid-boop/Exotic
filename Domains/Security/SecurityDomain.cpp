#include "SecurityDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void SecurityDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Security domain initialized." ^<^< std::endl;
}

void SecurityDomain::RegisterModules()
{
    if (!active) return;
}

void SecurityDomain::Shutdown()
{
    active = false;
}

}
