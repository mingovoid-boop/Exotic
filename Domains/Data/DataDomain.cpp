#include "DataDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void DataDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Data domain initialized." ^<^< std::endl;
}

void DataDomain::RegisterModules()
{
    if (!active) return;
}

void DataDomain::Shutdown()
{
    active = false;
}

}
