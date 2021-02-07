#pragma once

#include "../Serializable.h"
#include <nlohmann/json.hpp>

namespace core::traits
{
using JsonSerializer = Serializable<nlohmann::json>;
} // namespace core::traits