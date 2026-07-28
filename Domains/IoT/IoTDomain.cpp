#include "IoTDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void IoTDomain::Initialize()
{
    active = true;
    std::cout ^<^< "IoT domain initialized." ^<^< std::endl;
}

void IoTDomain::RegisterModules()
{
    if (!active) return;
}

void IoTDomain::Shutdown()
{
    active = false;
}

}
