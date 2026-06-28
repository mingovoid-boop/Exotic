#pragma once

#include "../foundation/include/Types.hpp"
#include <memory>
#include <unordered_map>

namespace exotic {
class IPlugin {
public:
virtual ~IPlugin() = default;
virtual String Name() const = 0;
virtual bool Initialize() = 0;
virtual void Shutdown() = 0;
};

class PluginManager {
public:
bool Register(std::shared_ptr<IPlugin> plugin);
std::shared_ptr<IPlugin> Find(const String& name);
void ShutdownAll();
private:
std::unordered_map<String,std::shared_ptr<IPlugin>> plugins_;
};
}
