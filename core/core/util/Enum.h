#pragma once

#include <boost/noncopyable.hpp>
#include <fmt/format.h>
#include <magic_enum.hpp>
#include <optional>

namespace util
{
class Enum : private boost::noncopyable
{
  public:
    template <typename T>
    constexpr static std::string_view toString(T type)
    {
        static_assert(std::is_enum_v<T>, "T is not an enum");
        return magic_enum::enum_name(type);
    }

    template <typename T>
    constexpr static std::optional<T> toEnum(std::string_view str)
    {
        static_assert(std::is_enum_v<T>, "T is not an enum");
        return magic_enum::enum_cast<T>(str);
    }

    template <typename T>
    constexpr static auto count()
    {
        static_assert(std::is_enum_v<T>, "T is not an enum");
        return magic_enum::enum_count<T>();
    }

    template <typename T>
    constexpr static auto iterate()
    {
        static_assert(std::is_enum_v<T>, "T is not an enum");
        return magic_enum::enum_values<T>();
    }

    template <typename T>
    constexpr static auto entries()
    {
        static_assert(std::is_enum_v<T>, "T is not an enum");
        return magic_enum::enum_entries<T>();
    }

  private:
    Enum() = default;
};
} // namespace util

using util::Enum;

#define REGISTER_ENUM(X)                                                   \
    template <>                                                            \
    struct fmt::formatter<X> : formatter<string_view>                      \
    {                                                                      \
        template <typename FormatContext>                                  \
        auto format(X e, FormatContext& ctx)                               \
        {                                                                  \
            return formatter<string_view>::format(Enum::toString(e), ctx); \
        }                                                                  \
    };
