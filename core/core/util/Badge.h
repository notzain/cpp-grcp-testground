#pragma once

namespace util
{
template <typename T>
class Badge
{
    friend T;
    Badge() = default;
};
} // namespace util

using util::Badge;