#pragma once

////////////////////////////////////////////////////////////
/// EXOTIC ENGINE
/// Engine: PredictionEngine
////////////////////////////////////////////////////////////

#include ^<string^>
#include ^<vector^>

namespace exotic
{

//==========================================================
// ENGINE
//==========================================================

class PredictionEngine
{
public:
    PredictionEngine();
    void Initialize();
    void Observe(const std::string^& input);
    void Update();
    void Shutdown();

private:
    bool initialized = false;
    std::vector^<std::string^> observations;
};

}
