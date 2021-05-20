#pragma once

#include <boost/range/adaptor/indexed.hpp>
#include <boost/type_traits.hpp>
#include <iterator>
#include <type_traits>
#include <utility>

namespace util
{
template <typename T>
class Sequence
{
    T first;
    T last;

  public:
    using Value = T;

    Sequence() = default;

    Sequence(T begin, T end)
        : first(std::move(begin))
        , last(std::move(end))
    {
    }

    struct Iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        mutable T value;

        Iterator(T v)
            : value(std::move(v))
        {
        }

        Iterator& operator++()
        {
            value = increment(value);
            return *this;
        }

        Iterator operator++(int) { return Iterator(value++); };

        bool operator==(const Iterator& rhs)
        {
            return value == rhs.value;
        }

        bool operator!=(const Iterator& rhs)
        {
            return !(*this == rhs);
        }

        reference operator*()
        {
            return value;
        }

        pointer operator->() const
        {
            return std::addressof(value);
        }
    };

    Iterator begin()
    {
        return Iterator(first);
    }
    Iterator end()
    {
        return Iterator(last);
    }

    static T increment(T value)
    {
        static_assert(boost::has_post_increment<T>::value, "T has to be incrementable (++ operator), or be template specialized.");
        static_assert(boost::has_equal_to<T>::value, "T has to be comparable (== operator), or be template specialized.");

        value++;
        return value;
    }

    template <template <typename> typename Cont, class Type = T>
    Cont<Type> to()
    {
        Cont<Type> cont;
        for (const auto self : *this)
        {
            cont.push_back(self);
        }

        return cont;
    }
};

template <typename T>
struct Inclusive
{
    T value;
    Inclusive(T t)
        : value(std::move(t))
    {
    }
    T operator()()
    {
        value = Sequence<T>::increment(value);
        return value;
    }
};

template <typename T>
struct Exclusive
{
    T value;
    Exclusive(T t)
        : value(std::move(t))
    {
    }
    T operator()()
    {
        value = Sequence<T>::increment(value);
        return value;
    }
};

/// @brief Iterate over a sequence, from beginning (inclusive) to end (exclusive)
/// @tparam Seq Sequence type
/// @param b Begin value
/// @param e End value
/// @return Sequence container
template <typename T>
Sequence<T> rangeOf(T begin, T end)
{
    return Sequence<T>(begin, end);
}

template <typename T, template <typename> typename Begin = Inclusive, template <typename> typename End = Inclusive>
Sequence<T> rangeOf(Begin<T> begin, End<T> end)
{
    if constexpr (std::is_same_v<Begin<T>, Exclusive<T>>)
        begin();

    if constexpr (std::is_same_v<End<T>, Inclusive<T>>)
        end();

    return Sequence<T>(begin.value, end.value);
}

template <typename T, template <typename> typename Begin>
Sequence<T> rangeOf(Begin<T> begin, T end)
{
    return Sequence<T>(begin(), end);
}

template <typename T, template <typename> typename End>
Sequence<T> rangeOf(T begin, End<T> end)
{
    return Sequence<T>(begin, end());
}

// returns iterator with `.value()` and `.index()`
// or deconstruct with `const auto& [index, value]`
template <typename T, template <typename> typename Container>
auto rangeIndexed(const Container<T>& input)
{
    return input | boost::adaptors::indexed(0);
}

} // namespace util