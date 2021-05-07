#pragma once

#include <nonstd/expected.hpp>

#include "Enum.h"

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

REGISTER_ENUM(util::Error::ErrorType);

using namespace util::Error;
using util::Result;