#pragma once

#include "core/util/Enum.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

namespace util
{
class ByteOrder
{

  public:
    enum Value
    {
        LittleEndian,
        BigEndian,
        HostOrder = LittleEndian,
        NetworkOrder = BigEndian
    };

  private:
    ByteOrder() = delete;

    template <typename T>
    class CopyFrom
    {
        friend ByteOrder;
        const T& m_input;
        std::optional<ByteOrder::Value> m_from;

      public:
        CopyFrom(const T& i)
            : m_input(i)
        {
        }

        [[nodiscard]] CopyFrom& from(ByteOrder::Value from)
        {
            m_from = from;
            return *this;
        }

        template <typename U = T>
        [[nodiscard]] U to(ByteOrder::Value to) const
        {
            if (!m_from.has_value())
                throw "ByteOrder::Copy 'from' not defined";

            if (m_from == to)
            {
                if constexpr (std::is_same_v<U, T>)
                    return m_input;

                U copy;
                std::copy(std::begin(m_input), std::end(m_input), std::begin(copy));
                return copy;
            }

            U copy;
            std::copy(std::rbegin(m_input), std::rend(m_input), std::begin(copy));
            return copy;
        }
    };

  public:
    template <typename T>
    static CopyFrom<T> copy(const T& input)
    {
        return CopyFrom<T>(input);
    }
};

} // namespace util

using util::ByteOrder;
REGISTER_ENUM(util::ByteOrder::Value);