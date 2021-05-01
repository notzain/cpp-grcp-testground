#pragma once

/* std */
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

/* external */
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/operators.hpp>
#include <boost/type_traits.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <nonstd/expected.hpp>
#include <nonstd/span.hpp>
// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/fmt.h>
// clang-format on

/* internal */
#include "core/util/Result.h"
#include "core/util/Sequence.h"
#include "core/util/logger/LogHelper.h"
#include "core/util/logger/Logger.h"
#include "core/traits/Serializable.h"
#include "core/traits/Printable.h"
#include "core/traits/serializer/JsonSerializer.h"
