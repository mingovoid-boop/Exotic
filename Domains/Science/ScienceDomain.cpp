#include "ScienceDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void ScienceDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Science domain initialized." ^<^< std::endl;
}

void ScienceDomain::RegisterModules()
{
    if (!active) return;
}

void ScienceDomain::Shutdown()
{
    active = false;
}

}
