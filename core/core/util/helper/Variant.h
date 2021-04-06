#pragma once

#include <variant>

namespace _detail
{
template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;
} // namespace _detail

template <typename Variant, typename... Cb>
void matchVariant(const Variant& v, Cb&&... cb)
{
    std::visit(_detail::overload{ std::forward<Cb>(cb)... }, v);
}