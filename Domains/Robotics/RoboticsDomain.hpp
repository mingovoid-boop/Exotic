#pragma once
#include ^<string^>

namespace exotic
{

//==========================================================
// DOMAIN
//==========================================================

class RoboticsDomain
{
public:
    void Initialize();
    void RegisterModules();
    void Shutdown();

private:
    std::string name = "Robotics";
    bool active = false;
};

}
