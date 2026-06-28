#include "EngineRegistry.hpp"

namespace exotic {
bool EngineRegistry::Register(std::shared_ptr<IEngine> engine) { if (!engine) return false; engines_[engine->Name()] = engine; return true; }
std::shared_ptr<IEngine> EngineRegistry::Find(const String& name) const { auto it = engines_.find(name); return it == engines_.end() ? nullptr : it->second; }
void EngineRegistry::InitializeAll() { for (auto& e : engines_) e.second->Initialize(); }
void EngineRegistry::UpdateAll() { for (auto& e : engines_) e.second->Update(); }
void EngineRegistry::ShutdownAll() { for (auto& e : engines_) e.second->Shutdown(); }
}
