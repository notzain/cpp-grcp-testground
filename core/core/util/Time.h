#pragma once

#include <chrono>

using SystemClock = std::chrono::system_clock;
using TimePoint = SystemClock::time_point;
using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
