#pragma once

#include <spdlog/fmt/ostr.h> // must be included
#include <string>

namespace core::traits
{
template <typename Self>
struct Printable
{
    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Self &self)
    {
        return os << self.format();
    }

    virtual std::string format() const = 0;
};

template <typename Type>
struct Printer
{
    const Type &type;
    Printer(const Type &t)
        : type(t)
    {
    }

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const Printer &self)
    {
        return os << self.format();
    }

    virtual std::string format() const = 0;
};
} // namespace core::traits