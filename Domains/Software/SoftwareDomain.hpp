#pragma once
#include ^<string^>

namespace exotic
{

//==========================================================
// DOMAIN
//==========================================================

class SoftwareDomain
{
public:
    void Initialize();
    void RegisterModules();
    void Shutdown();

private:
    std::string name = "Software";
    bool active = false;
};

}
