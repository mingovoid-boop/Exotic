#include "PluginManager.hpp"

namespace exotic {
bool PluginManager::Register(std::shared_ptr<IPlugin> plugin) { if (!plugin) return false; plugins_[plugin->Name()] = plugin; return plugin->Initialize(); }
std::shared_ptr<IPlugin> PluginManager::Find(const String& name) { auto it = plugins_.find(name); return it == plugins_.end() ? nullptr : it->second; }
void PluginManager::ShutdownAll() { for (auto& p : plugins_) p.second->Shutdown(); plugins_.clear(); }
}
