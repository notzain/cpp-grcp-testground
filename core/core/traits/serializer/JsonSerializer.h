#pragma once

#include "../Serializable.h"
#include <nlohmann/json.hpp>

namespace traits
{
using JsonSerializer = Serializable<nlohmann::json>;
} // namespace traits
