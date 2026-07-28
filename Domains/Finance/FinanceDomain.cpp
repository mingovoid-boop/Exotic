#include "FinanceDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void FinanceDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Finance domain initialized." ^<^< std::endl;
}

void FinanceDomain::RegisterModules()
{
    if (!active) return;
}

void FinanceDomain::Shutdown()
{
    active = false;
}

}
