#pragma once

#include <boost/operators.hpp>
#include <boost/type_traits.hpp>
#include <limits>

namespace core::util
{
template <typename T,
          T Min = (std::numeric_limits<T>::min)(),
          T Max = (std::numeric_limits<T>::max)()>
class WrapAround
    : boost::addable<WrapAround<T, Min, Max>>
    , boost::subtractable<WrapAround<T, Min, Max>>
    , boost::dividable<WrapAround<T, Min, Max>>
    , boost::multipliable<WrapAround<T, Min, Max>>
    , boost::incrementable<WrapAround<T, Min, Max>>
    , boost::decrementable<WrapAround<T, Min, Max>>
{
    static_assert(boost::is_arithmetic<T>::value, "T must be an arithmetic type.");
    T m_value;

  public:
    WrapAround()
        : m_value(Min)
    {
    }

    explicit WrapAround(T value)
        : m_value(Min)
    {
    }

    T value() const
    {
        return m_value;
    }

    T operator*() const
    {
        return m_value;
    }

    WrapAround& operator++()
    {
        wrap(++m_value);
        return *this;
    }

    WrapAround& operator--()
    {
        wrap(--m_value);
        return *this;
    }

    WrapAround operator+=(const WrapAround& rhs)
    {
        return { m_value + rhs.m_value };
    }

    WrapAround operator-=(const WrapAround& rhs)
    {
        return { m_value - rhs.m_value };
    }

    WrapAround operator*=(const WrapAround& rhs)
    {
        return { m_value * rhs.m_value };
    }

    WrapAround operator/=(const WrapAround& rhs)
    {
        return { m_value / rhs.m_value };
    }

    bool operator==(const WrapAround& rhs) const
    {
        return m_value == rhs.m_value;
    }

    bool operator!=(const WrapAround& rhs) const
    {
        return !(rhs == *this);
    }

    bool operator<(const WrapAround& rhs) const
    {
        return m_value < rhs.m_value;
    }

    bool operator>(const WrapAround& rhs) const
    {
        return rhs < *this;
    }

    bool operator<=(const WrapAround& rhs) const
    {
        return !(rhs < *this);
    }

    bool operator>=(const WrapAround& rhs) const
    {
        return !(*this < rhs);
    }

  private:
    void wrap(T value)
    {
        // If the wrap around limits are the data types natural min and max,
        // we can take advantage of the hardware over/underflow to wrap.
        if constexpr (Min == (std::numeric_limits<T>::min)() && Max == (std::numeric_limits<T>::max)())
        {
            m_value = value;
            return;
        }

        if (value < Min)
        {
            m_value = Max;
        }
        else if (value > Max)
        {
            m_value = Min;
        }
        else
        {
            m_value = value;
        }
    }
};
} // namespace core::util