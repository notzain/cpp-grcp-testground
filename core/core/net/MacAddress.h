#pragma once

#include "core/traits/Printable.h"
#include "core/util/ByteOrder.h"
#include "core/util/Result.h"

#include <array>
#include <cstdint>
#include <initializer_list>
#include <nonstd/span.hpp>
#include <string>
#include <string_view>

namespace pcpp
{
class MacAddress;
}
namespace net
{
class MacAddress : public traits::Printable<MacAddress>
{
    // Stored in network order
    std::array<std::uint8_t, 6> m_macAsBytes = { 0 };

  public:
    MacAddress(const pcpp::MacAddress& mac);
    MacAddress() = default;

    static Result<MacAddress> parse(std::array<std::uint8_t, 6> bytes, ByteOrder::Value byteOrder = ByteOrder::HostOrder);
    static Result<MacAddress> parse(std::initializer_list<std::uint8_t> bytes, ByteOrder::Value byteOrder = ByteOrder::HostOrder);
    static Result<MacAddress> parse(nonstd::span<std::uint8_t> bytes, ByteOrder::Value byteOrder = ByteOrder::HostOrder);
    static Result<MacAddress> parse(std::string_view ip);
    static Result<MacAddress> parse(std::uint64_t number);

    static const MacAddress& broadcast();
    static const MacAddress& zero();

    bool isBetween(const MacAddress& begin, const MacAddress& end) const;
    bool isBroadcast(const MacAddress& mac) const;

    std::array<std::uint8_t, 6> toBytes(ByteOrder::Value byteOrder = ByteOrder::HostOrder) const;
    std::uint64_t toInt() const;
    std::string toString() const override;

    bool operator==(const MacAddress& other) const { return m_macAsBytes == other.m_macAsBytes; }
    bool operator!=(const MacAddress& other) const { return !(*this == other); }
    bool operator<(const MacAddress& other) const { return toInt() < other.toInt(); }
    bool operator>(const MacAddress& other) const { return other < *this; }
    bool operator<=(const MacAddress& other) const { return !(other < *this); }
    bool operator>=(const MacAddress& other) const { return !(*this < other); }

    bool operator==(std::string_view other) const { return toString() == other; }
    bool operator!=(std::string_view other) const { return !(*this == other); }

    MacAddress operator++(int);

  private:
    MacAddress(std::array<std::uint8_t, 6> bytes);
};
} // namespace net