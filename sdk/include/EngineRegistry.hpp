#pragma once

#include "../foundation/include/Types.hpp"
#include <unordered_map>
#include <memory>

namespace exotic {
class IEngine {
public:
    virtual ~IEngine() = default;
    virtual String Name() const = 0;
    virtual bool Initialize() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
};

class EngineRegistry {
public:
    bool Register(std::shared_ptr<IEngine> engine);
    std::shared_ptr<IEngine> Find(const String& name) const;
    void InitializeAll();
    void UpdateAll();
    void ShutdownAll();
private:
    std::unordered_map<String,std::shared_ptr<IEngine>> engines_;
};
}
