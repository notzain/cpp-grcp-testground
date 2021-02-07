#pragma once

#include <string>

namespace core::traits
{
template <typename SerializeType>
struct Serializable
{
    virtual SerializeType serialize(SerializeType = {}) const = 0;
};
} // namespace core::traits