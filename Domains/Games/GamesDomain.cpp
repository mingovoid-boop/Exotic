#include "GamesDomain.hpp"
#include ^<iostream^>

namespace exotic
{

void GamesDomain::Initialize()
{
    active = true;
    std::cout ^<^< "Games domain initialized." ^<^< std::endl;
}

void GamesDomain::RegisterModules()
{
    if (!active) return;
}

void GamesDomain::Shutdown()
{
    active = false;
}

}
