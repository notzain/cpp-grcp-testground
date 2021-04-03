#pragma once

#include <boost/operators.hpp>
#include <boost/type_traits.hpp>
#include <limits>

namespace util
{
template <typename T,
          T MinValue = (std::numeric_limits<T>::min)(),
          T MaxValue = (std::numeric_limits<T>::max)()>
class WrapAround
    : boost::incrementable<WrapAround<T, MinValue, MaxValue>>
    , boost::decrementable<WrapAround<T, MinValue, MaxValue>>
{
    static_assert(boost::is_arithmetic<T>::value, "T must be an arithmetic type.");
    T m_value;

  public:
    constexpr static T Min = MinValue;
    constexpr static T Max = MaxValue;

    WrapAround()
        : m_value(MinValue)
    {
    }

    explicit WrapAround(T value)
        : m_value(MinValue)
    {
        wrap(value);
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

    template <typename U>
    bool operator==(const U& rhs) const
    {
        return m_value == rhs;
    }

    template <typename U>
    bool operator!=(const U& rhs) const
    {
        return m_value != rhs;
    }

    template <typename U>
    bool operator<(const U& rhs) const
    {
        return m_value < rhs;
    }

    template <typename U>
    bool operator>(const U& rhs) const
    {
        return m_value > rhs;
    }

    template <typename U>
    bool operator<=(const U& rhs) const
    {
        return !(rhs < *this);
    }

    template <typename U>
    bool operator>=(const U& rhs) const
    {
        return !(*this < rhs);
    }

  private:
    void wrap(T value)
    {
        // If the wrap around limits are the data types natural min and max,
        // we can take advantage of the hardware over/underflow to wrap.
        if constexpr (MinValue == (std::numeric_limits<T>::min)() && MaxValue == (std::numeric_limits<T>::max)())
        {
            m_value = value;
            return;
        }

        if (value < MinValue)
        {
            m_value = MaxValue;
        }
        else if (value > MaxValue)
        {
            m_value = MinValue;
        }
        else
        {
            m_value = value;
        }
    }
};
} // namespace util