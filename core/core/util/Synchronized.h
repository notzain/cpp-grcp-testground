#pragma once

#include <boost/thread/synchronized_value.hpp>

namespace util
{
template <typename T>
using Synchronized = boost::synchronized_value<T>;

template <typename... Ts>
auto synchronizeAll(Ts&... ts)
{
    return boost::synchronize(ts...);
}
} // namespace util

using util::synchronizeAll;
using util::Synchronized;