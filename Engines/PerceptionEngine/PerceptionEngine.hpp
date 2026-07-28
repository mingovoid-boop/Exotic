#pragma once

////////////////////////////////////////////////////////////
/// EXOTIC XIR ENGINE
/// Engine: PerceptionEngine
/// Source: XIR Node
////////////////////////////////////////////////////////////

#include ^<string^>
#include ^<vector^>

namespace exotic
{

class PerceptionEngine
{
public:
    PerceptionEngine();
    void Initialize();
    void Observe(const std::string^& input);
    void Update();
    void Shutdown();

private:
    bool initialized = false;
    std::vector^<std::string^> observations;
};

}
