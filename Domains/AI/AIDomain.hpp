#pragma once
#include ^<string^>

namespace exotic
{

//==========================================================
// DOMAIN
//==========================================================

class AIDomain
{
public:
    void Initialize();
    void RegisterModules();
    void Shutdown();

private:
    std::string name = "AI";
    bool active = false;
};

}
