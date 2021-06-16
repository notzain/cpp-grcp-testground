#pragma once

#include "core/util/logger/Logger.h"
#include <cassert>

#ifdef NDEBUG
#define Verify(x)                                      \
    do                                                 \
    {                                                  \
        if (!x)                                        \
            CORE_WARN("Verification '{}' failed", #x); \
    } while (0)

#define Assert(x)                                   \
    do                                              \
    {                                               \
        if (!x)                                     \
        {                                           \
            CORE_WARN("Assertion '{}' failed", #x); \
        }                                           \
    } while (0)
#else
#define Verify(x)                                      \
    do                                                 \
    {                                                  \
        if (!x)                                        \
            CORE_WARN("Verification '{}' failed", #x); \
    } while (0)

#define Assert(x)                                   \
    do                                              \
    {                                               \
        if (!x)                                     \
        {                                           \
            CORE_WARN("Assertion '{}' failed", #x); \
            std::abort();                           \
        }                                           \
    } while (0)
#endif