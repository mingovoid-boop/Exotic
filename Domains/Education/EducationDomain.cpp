#include "EducationDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void EducationDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Education domain initialized." ^<^< std::endl;
}

void EducationDomain::RegisterModules()
{
    if (!active) return;
}

void EducationDomain::Shutdown()
{
    active = false;
}

}
