#include "CreativeDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void CreativeDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Creative domain initialized." ^<^< std::endl;
}

void CreativeDomain::RegisterModules()
{
    if (!active) return;
}

void CreativeDomain::Shutdown()
{
    active = false;
}

}
