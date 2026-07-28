#include "RoboticsDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void RoboticsDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Robotics domain initialized." ^<^< std::endl;
}

void RoboticsDomain::RegisterModules()
{
    if (!active) return;
}

void RoboticsDomain::Shutdown()
{
    active = false;
}

}
