#pragma once

#include "Reflection.hpp"
#include "../foundation/include/Types.hpp"

namespace exotic {
class ISerializable : public IReflectable {
public:
    virtual ~ISerializable() = default;
    virtual String ToJson() const = 0;
    virtual bool FromJson(const String& json) = 0;
    virtual String ToBinary() const = 0;
    virtual bool FromBinary(const String& data) = 0;
};
}
