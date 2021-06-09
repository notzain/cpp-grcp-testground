#pragma once

#include "core/traits/Printable.h"
#include <sstream>
#include <type_traits>

namespace _detail
{
// To allow ADL with custom begin/end
using std::begin;
using std::end;

template <typename T>
constexpr auto is_iterable_impl(int)
    -> decltype(
        begin(std::declval<T&>()) != end(std::declval<T&>()),   // begin/end and operator !=
        void(),                                                 // Handle evil operator ,
        ++std::declval<decltype(begin(std::declval<T&>()))&>(), // operator ++
        void(*begin(std::declval<T&>())),                       // operator*
        std::true_type{});

template <typename T>
std::false_type is_iterable_impl(...);

template <typename T>
using is_iterable = decltype(is_iterable_impl<T>(0));
template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
struct is_pair : std::false_type
{
};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type
{
};

template <typename T>
constexpr bool is_pair_v = is_pair<T>::value;

template <typename, typename = void>
struct is_mapping : std::false_type
{
};

template <typename Container>
struct is_mapping<Container, std::enable_if_t<is_pair_v<typename std::iterator_traits<typename Container::iterator>::value_type>>> : std::true_type
{
};

template <typename T>
constexpr bool is_mapping_v = is_mapping<T>::value;
} // namespace _detail

template <typename T>
struct MapPrinter : public traits::Printer<T>
{
    using traits::Printer<T>::Printer;
    std::string toString() const override
    {
        std::stringstream ss;
        const auto last = std::prev(this->m_type.end());
        for (auto it = this->m_type.begin(); it != this->m_type.end(); ++it)
        {
            const auto& [k, v] = *it;
            ss << "{ " << k << " : " << v << " }";
            if (it != last)
                ss << ", ";
        }
        return ss.str();
    }
};
