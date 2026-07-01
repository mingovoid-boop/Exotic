#pragma once

namespace exotic {

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
};

}

