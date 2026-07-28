#pragma once

////////////////////////////////////////////////////////////
/// EXOTIC ENGINE
/// Engine: AlignmentEngine
////////////////////////////////////////////////////////////

#include ^<string^>
#include ^<vector^>

namespace exotic
{

//==========================================================
// ENGINE
//==========================================================

class AlignmentEngine
{
public:
    AlignmentEngine();
    void Initialize();
    void Observe(const std::string^& input);
    void Update();
    void Shutdown();

private:
    bool initialized = false;
    std::vector^<std::string^> observations;
};

}
