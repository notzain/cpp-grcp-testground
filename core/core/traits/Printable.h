#pragma once

#include <spdlog/fmt/ostr.h> // must be included
#include <string>

namespace traits
{
template <typename Self>
struct Printable
{
    template <typename OStream>
    friend OStream& operator<<(OStream& os, const Self& self)
    {
        return os << self.toString();
    }

    friend std::ostream& operator<<(std::ostream& os, const Self& self)
    {
        return os << self.toString();
    }

    virtual std::string toString() const = 0;
};

template <typename Type>
struct Printer
{
    const Type& m_type;
    Printer(const Type& t)
        : m_type(t)
    {
    }

    template <typename OStream>
    friend OStream& operator<<(OStream& os, const Printer& self)
    {
        return os << self.toString();
    }

    virtual std::string toString() const = 0;
};
} // namespace traits

template <typename T>
struct fmt::formatter<traits::Printable<T>> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const traits::Printable<T>& printable, FormatContext& ctx)
    {
        return fmt::formatter<std::string>::format(printable.toString(), ctx);
    }
};

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<traits::Printable<T>, T>::value, char>> : fmt::formatter<std::string>
{
    template <typename FormatCtx>
    auto format(const traits::Printable<T>& printable, FormatCtx& ctx)
    {
        return fmt::formatter<std::string>::format(printable.toString(), ctx);
    }
};