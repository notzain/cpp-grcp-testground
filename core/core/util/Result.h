#pragma once

#include <nonstd/expected.hpp>

#include <string>
#include <type_traits>

namespace core::util
{
template <typename T = void, typename Error = std::string>
class Result : public nonstd::expected<T, Error>
{
  public:
    using nonstd::expected<T, Error>::expected;

    template <typename E>
    static auto unexpected(E&& err)
    {
        return nonstd::make_unexpected<E>(std::move(err));
    }
};
} // namespace core::util
