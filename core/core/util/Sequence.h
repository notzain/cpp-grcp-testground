#pragma once

#include <boost/type_traits.hpp>
#include <iterator>
#include <utility>

namespace core::util
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
        using pointer = T *;
        using reference = T &;

        Sequence *seq;
        mutable T value;

        Iterator(Sequence* sequence, T v)
            : seq(sequence)
            , value(std::move(v))
        {
        }

        Iterator &operator++()
        {
            value = seq->increment(value);
            return *this;
        }

        Iterator operator++(int) = delete;

        bool operator==(const Iterator &rhs)
        {
            return value == rhs.value;
        }

        bool operator!=(const Iterator &rhs)
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
        return Iterator(this, first);
    }
    Iterator end()
    {
        return Iterator(this, last);
    }

    T increment(const T &value) const
    {
        static_assert(boost::has_pre_increment<T>::value, "T has to be incrementable (++ operator), or be template specialized.");
        static_assert(boost::has_equal_to<T>::value, "T has to be comparable (== operator), or be template specialized.");

        T v = value;
        return ++v;
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

/// @brief Iterate over a sequence, from beginning (inclusive) to end (inclusive)
/// @tparam Seq Sequence type
/// @param b Begin value
/// @param e End value
/// @return Sequence container
template <typename T>
Sequence<T> rangeOf(T begin, T end)
{
    return Sequence<T>(begin, Sequence<T>{}.increment(end));
}

} // namespace core::util