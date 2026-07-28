#include "PersonalDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void PersonalDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Personal domain initialized." ^<^< std::endl;
}

void PersonalDomain::RegisterModules()
{
    if (!active) return;
}

void PersonalDomain::Shutdown()
{
    active = false;
}

}
