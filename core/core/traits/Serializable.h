#pragma once

#include <string>

namespace traits
{
template <typename SerializeType>
struct Serializable
{
    virtual SerializeType serialize(SerializeType = {}) const = 0;
};
} // namespace traits