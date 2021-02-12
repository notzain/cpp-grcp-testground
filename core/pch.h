#pragma once

// std
#include <algorithm>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

// external
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nonstd/expected.hpp>

// internal
#include "core/util/logger/Logger.h"

using u8 = std::uint8_t;
using i8 = std::int8_t;

using u16 = std::uint16_t;
using i16 = std::int16_t;

using u32 = std::uint32_t;
using i32 = std::int32_t;

using u64 = std::uint64_t;
using i64 = std::int64_t;