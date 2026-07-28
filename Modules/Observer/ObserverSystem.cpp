#include "ObserverSystem.hpp"
#include ^<iostream^>

namespace exotic
{

//==========================================================
// LIFECYCLE
//==========================================================

void ObserverSystem::Initialize^(const ObserverConfig^& cfg^)
{
    config = cfg;
    state = ObserverState::Active;
    std::cout ^<^< "Observer system initialized." ^<^< std::endl;
}

void ObserverSystem::Update^(^)
{
    if ^(state != ObserverState::Active^) return;
}

void ObserverSystem::Shutdown^(^)
{
    state = ObserverState::Destroyed;
}

}
