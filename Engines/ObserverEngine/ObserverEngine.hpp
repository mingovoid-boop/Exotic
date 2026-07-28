#pragma once

////////////////////////////////////////////////////////////
/// EXOTIC ENGINE
/// Engine: ObserverEngine
////////////////////////////////////////////////////////////

#include ^<string^>
#include ^<vector^>

namespace exotic
{

//==========================================================
// ENGINE
//==========================================================

class ObserverEngine
{
public:
    ObserverEngine();
    void Initialize();
    void Observe(const std::string^& input);
    void Update();
    void Shutdown();

private:
    bool initialized = false;
    std::vector^<std::string^> observations;
};

}
