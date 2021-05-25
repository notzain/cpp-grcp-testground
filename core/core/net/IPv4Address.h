#pragma once

#include "core/traits/Printable.h"
#include "core/util/ByteOrder.h"
#include "core/util/Result.h"

#include <array>
#include <boost/operators.hpp>
#include <cstdint>
#include <initializer_list>
#include <nonstd/span.hpp>
#include <string>
#include <string_view>

namespace pcpp
{
class IPv4Address;
}

namespace net
{
class IPv4Address
    : public traits::Printable<IPv4Address>
{
    // Stored in network order
    std::array<std::uint8_t, 4> m_ipAsBytes{ 0 };

  public:
    // Allow conversion from pcpp
    IPv4Address(const pcpp::IPv4Address& ip);
    IPv4Address() = default;
    static Result<IPv4Address> parse(std::array<std::uint8_t, 4> bytes, ByteOrder::Value byteOrder = ByteOrder::HostOrder);
    static Result<IPv4Address> parse(std::initializer_list<std::uint8_t> bytes, ByteOrder::Value byteOrder = ByteOrder::HostOrder);
    static Result<IPv4Address> parse(nonstd::span<std::uint8_t> bytes, ByteOrder::Value byteOrder = ByteOrder::HostOrder);
    static Result<IPv4Address> parse(std::string_view ip);
    static Result<IPv4Address> parse(std::uint32_t number);

    static const IPv4Address& zero();
    static const IPv4Address& broadcast();
    static const IPv4Address& loopback();

    bool isBetween(const IPv4Address& begin, const IPv4Address& end) const;
    bool isLoopback() const;

    std::string asString() const;
    std::array<std::uint8_t, 4> asBytes(ByteOrder::Value byteOrder = ByteOrder::HostOrder) const;
    std::uint32_t asInt() const;

    std::string format() const override;

    bool operator==(const IPv4Address& other) const { return m_ipAsBytes == other.m_ipAsBytes; }
    bool operator!=(const IPv4Address& other) const { return !(*this == other); }
    bool operator<(const IPv4Address& other) const { return asInt() < other.asInt(); }
    bool operator>(const IPv4Address& other) const { return other < *this; }
    bool operator<=(const IPv4Address& other) const { return !(other < *this); }
    bool operator>=(const IPv4Address& other) const { return !(*this < other); }

    bool operator==(std::string_view other) const { return asString() == other; }
    bool operator!=(std::string_view other) const { return !(*this == other); }

    IPv4Address operator++(int)
    {
        auto prev = *this;
        m_ipAsBytes = IPv4Address::parse(this->asInt() + 1)->m_ipAsBytes;
        return prev;
    }

  private:
    IPv4Address(std::array<std::uint8_t, 4> bytes);
};

} // namespace net