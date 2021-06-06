#pragma once

namespace util
{
namespace detail
{
template <typename F>
struct Defer
{
    F f;
    Defer(F f)
        : f(f)
    {
    }
    ~Defer() { f(); }
};

template <typename F>
Defer<F> defer(F f)
{
    return Defer<F>(f);
}
} // namespace detail
} // namespace util

#define UTIL_DEFER_1(x, y) x##y
#define UTIL_DEFER_2(x, y) UTIL_DEFER_1(x, y)
#define UTIL_DEFER_3(x) UTIL_DEFER_2(x, __COUNTER__)
#define defer(code) auto UTIL_DEFER_3(_defer_) = util::detail::defer([&]() { code; })
