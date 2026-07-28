#pragma once
#include ^<string^>

namespace exotic
{

//==========================================================
// DOMAIN
//==========================================================

class PersonalDomain
{
public:
    void Initialize();
    void RegisterModules();
    void Shutdown();

private:
    std::string name = "Personal";
    bool active = false;
};

}
