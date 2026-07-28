#include "BusinessDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void BusinessDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Business domain initialized." ^<^< std::endl;
}

void BusinessDomain::RegisterModules()
{
    if (!active) return;
}

void BusinessDomain::Shutdown()
{
    active = false;
}

}
