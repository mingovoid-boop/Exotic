#pragma once
#include "ObserverConfig.hpp"
#include "ObserverTypes.hpp"

namespace exotic
{

//==========================================================
// SYSTEM
//==========================================================

class ObserverSystem
{
public:
    void Initialize^(const ObserverConfig^& config^);
    void Update^(^);
    void Shutdown^(^);

private:
    ObserverConfig config;
    ObserverState state = ObserverState::Created;
};

}
