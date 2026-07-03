#pragma once
#include "ModuleInfo.hpp
namespace exotic {
class IModule {
public:
 virtual ~IModule() = default;
 virtual ModuleInfo Info() const = 0;
 virtual bool Initialize() = 0;
 virtual bool Validate() const = 0;
 virtual void Shutdown() = 0;
};
}
