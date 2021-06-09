#include "IPv4Address.h"
#include "core/util/ByteOrder.h"
#include "core/util/Result.h"

#include <IpAddress.h>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstdint>
#include <initializer_list>

namespace net
{
namespace
{

} // namespace

IPv4Address::IPv4Address(std::array<std::uint8_t, 4> bytes)
    : m_ipAsBytes(std::move(bytes))
{
}

IPv4Address::IPv4Address(const pcpp::IPv4Address& ip)
{
    m_ipAsBytes = ByteOrder::copy(nonstd::span(ip.toBytes(), 4))
                      .from(ByteOrder::HostOrder)
                      .to<std::array<std::uint8_t, 4>>(ByteOrder::NetworkOrder);
}

Result<IPv4Address> IPv4Address::parse(std::array<std::uint8_t, 4> bytes, ByteOrder::Value byteOrder)
{
    return IPv4Address(ByteOrder::copy(bytes)
                           .from(byteOrder)
                           .to(ByteOrder::NetworkOrder));
}

Result<IPv4Address> IPv4Address::parse(std::initializer_list<std::uint8_t> bytes, ByteOrder::Value byteOrder)
{
    if (bytes.size() != 4)
        return Error(ErrorType::InvalidArgument);

    return IPv4Address(ByteOrder::copy(bytes)
                           .from(byteOrder)
                           .to<std::array<std::uint8_t, 4>>(ByteOrder::NetworkOrder));
}

Result<IPv4Address> IPv4Address::parse(nonstd::span<std::uint8_t> bytes, ByteOrder::Value byteOrder)
{
    if (bytes.size() != 4)
        return Error(ErrorType::InvalidArgument);

    return IPv4Address(ByteOrder::copy(bytes)
                           .from(byteOrder)
                           .to<std::array<std::uint8_t, 4>>(ByteOrder::NetworkOrder));
}

Result<IPv4Address> IPv4Address::parse(std::string_view ip)
{
    boost::system::error_code ec;
    auto address = boost::asio::ip::make_address_v4(ip, ec);
    if (ec)
        return Error(ErrorType::InvalidArgument);

    return IPv4Address(ByteOrder::copy(address.to_bytes())
                           .from(ByteOrder::HostOrder)
                           .to<std::array<std::uint8_t, 4>>(ByteOrder::NetworkOrder));
}

Result<IPv4Address> IPv4Address::parse(std::uint32_t number)
{
    std::array<std::uint8_t, 4> ip;
    ip[0] = number & 0xFF;
    ip[1] = (number >> 8) & 0xFF;
    ip[2] = (number >> 16) & 0xFF;
    ip[3] = (number >> 24) & 0xFF;

    return IPv4Address(ip);
}

const IPv4Address& IPv4Address::zero()
{
    static auto zero = *IPv4Address::parse(0);
    return zero;
}

const IPv4Address& IPv4Address::broadcast()
{
    static auto broadcast = *IPv4Address::parse({ 255, 255, 255, 255 });
    return broadcast;
}

const IPv4Address& IPv4Address::loopback()
{
    static auto loopback = *IPv4Address::parse({ 127, 0, 0, 1 });
    return loopback;
}

const IPv4Address& IPv4Address::localhost()
{
    return loopback();
}

bool IPv4Address::isBetween(const IPv4Address& begin, const IPv4Address& end) const
{
    if (begin > end)
        return isBetween(end, begin);
    return *this > begin && *this < end;
}

bool IPv4Address::isZero() const
{
    return *this == zero();
}

bool IPv4Address::isBroadcast() const
{
    return *this == broadcast();
}

bool IPv4Address::isLoopback() const
{
    return *this == loopback();
}

bool IPv4Address::isLocalhost() const
{
    return isLoopback();
}

std::string IPv4Address::asString() const
{
    return fmt::format("{}", fmt::join(m_ipAsBytes.rbegin(), m_ipAsBytes.rend(), "."));
}

std::array<std::uint8_t, 4> IPv4Address::asBytes(ByteOrder::Value byteOrder) const
{
    return ByteOrder::copy(m_ipAsBytes)
        .from(ByteOrder::NetworkOrder)
        .to(byteOrder);
}

std::uint32_t IPv4Address::asInt() const
{
    std::uint32_t addr;
    memcpy(&addr, m_ipAsBytes.data(), m_ipAsBytes.size());
    return addr;
}

std::string IPv4Address::toString() const
{
    return asString();
}

IPv4Address IPv4Address::operator++(int)
{
    auto prev = *this;
    m_ipAsBytes = IPv4Address::parse(this->asInt() + 1)->m_ipAsBytes;
    return prev;
}
} // namespace net
