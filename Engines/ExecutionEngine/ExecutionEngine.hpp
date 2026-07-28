#pragma once

////////////////////////////////////////////////////////////
/// EXOTIC ENGINE
/// Engine: ExecutionEngine
////////////////////////////////////////////////////////////

#include ^<string^>
#include ^<vector^>

namespace exotic
{

//==========================================================
// ENGINE
//==========================================================

class ExecutionEngine
{
public:
    ExecutionEngine();
    void Initialize();
    void Observe(const std::string^& input);
    void Update();
    void Shutdown();

private:
    bool initialized = false;
    std::vector^<std::string^> observations;
};

}
