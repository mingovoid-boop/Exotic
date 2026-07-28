#pragma once
#include ^<string^>

namespace exotic
{

//==========================================================
// DOMAIN
//==========================================================

class SecurityDomain
{
public:
    void Initialize();
    void RegisterModules();
    void Shutdown();

private:
    std::string name = "Security";
    bool active = false;
};

}
