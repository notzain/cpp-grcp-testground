#pragma once

#include <nonstd/expected.hpp>

#include <string>
#include <type_traits>

namespace core::util
{
template <typename T, typename Error = std::string>
class Result : public nonstd::expected<T, Error>
{
  public:
    using nonstd::expected<T, Error>::expected;

    static auto unexpected(Error&& err)
    {
        return nonstd::make_unexpected<Error>(std::move(err));
    }
};
} // namespace core::util
