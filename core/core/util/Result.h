#pragma once

#include <fmt/format.h>
#include <magic_enum.hpp>
#include <nonstd/expected.hpp>

namespace util
{
namespace Error
{
enum class ErrorType
{
    Aborted,
    AlreadyExists,
    Cancelled,
    DeadlineExceeded,
    FailedPrecondition,
    InvalidArgument,
    NotFound,
    OutOfRange,
    PermissionDenied,
    ResourceExhausted,
    TimedOut,
    Unauthenticated,
    Unavailable,
    Unimplemented,
    Unknown,
};

template <typename Err>
inline auto Error(Err&& err)
{
    return nonstd::make_unexpected<Err>(std::move(err));
}

inline auto Error(ErrorType&& err) { return nonstd::make_unexpected<ErrorType>(std::move(err)); }

}; // namespace Error

template <typename T, typename Err = Error::ErrorType>
class Result : public nonstd::expected<T, Err>
{
  public:
    using nonstd::expected<T, Err>::expected;
};

} // namespace util

template <>
struct fmt::formatter<util::Error::ErrorType> : formatter<string_view>
{
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(util::Error::ErrorType e, FormatContext& ctx)
    {
        return formatter<string_view>::format(magic_enum::enum_name(e), ctx);
    }
};

using namespace util::Error;
using util::Result;