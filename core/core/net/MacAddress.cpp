#include "MacAddress.h"

#include <MacAddress.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <cstdint>
#include <nonstd/span.hpp>

namespace net
{

MacAddress::MacAddress(const pcpp::MacAddress& mac)
{
    m_macAsBytes = ByteOrder::copy(nonstd::span(mac.getRawData(), 6))
                       .from(ByteOrder::HostOrder)
                       .to<std::array<std::uint8_t, 6>>(ByteOrder::NetworkOrder);
}

MacAddress::MacAddress(std::array<std::uint8_t, 6> bytes)
    : m_macAsBytes(std::move(bytes))
{
}

Result<MacAddress> MacAddress::parse(std::array<std::uint8_t, 6> bytes, ByteOrder::Value byteOrder)
{
    return MacAddress(ByteOrder::copy(bytes)
                          .from(byteOrder)
                          .to(ByteOrder::NetworkOrder));
}

Result<MacAddress> MacAddress::parse(std::initializer_list<std::uint8_t> bytes, ByteOrder::Value byteOrder)
{
    if (bytes.size() != 6)
        return Error(ErrorType::InvalidArgument);

    return MacAddress(ByteOrder::copy(bytes)
                          .from(byteOrder)
                          .to<std::array<std::uint8_t, 6>>(ByteOrder::NetworkOrder));
}

Result<MacAddress> MacAddress::parse(nonstd::span<std::uint8_t> bytes, ByteOrder::Value byteOrder)
{
    if (bytes.size() != 6)
        return Error(ErrorType::InvalidArgument);

    return MacAddress(ByteOrder::copy(bytes)
                          .from(byteOrder)
                          .to<std::array<std::uint8_t, 6>>(ByteOrder::NetworkOrder));
}

Result<MacAddress> MacAddress::parse(std::string_view mac)
{
    // FIXME: No input sanitizing, insecure sscanf. Should do better.
    static auto pattern = fmt::format("%hhx{0}%hhx{0}%hhx{0}%hhx{0}%hhx{0}%hhx%n", "%*1[-: ]");
    std::array<std::uint8_t, 6> bytes;
    int n = 0;
    if (std::sscanf(mac.data(), pattern.data(), &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5], &n) == 6 &&
        mac[n] == '\0')
    {
        return MacAddress(ByteOrder::copy(bytes)
                              .from(ByteOrder::HostOrder)
                              .to(ByteOrder::NetworkOrder));
    }

    return Error(ErrorType::InvalidArgument);
}

Result<MacAddress> MacAddress::parse(std::uint64_t number)
{
    // every uint64 value is a possible MAc address
    std::array<std::uint8_t, 6> mac;
    mac[0] = number & 0xFF;
    mac[1] = (number >> 8) & 0xFF;
    mac[2] = (number >> 16) & 0xFF;
    mac[3] = (number >> 24) & 0xFF;
    mac[4] = (number >> 32) & 0xFF;
    mac[5] = (number >> 40) & 0xFF;

    return MacAddress(mac);
}

const MacAddress& MacAddress::broadcast()
{
    static auto broadcast = *MacAddress::parse("FF:FF:FF:FF:FF:FF");
    return broadcast;
}

const MacAddress& MacAddress::zero()
{
    static auto zero = *MacAddress::parse("00:00:00:00:00:00");
    return zero;
}

bool MacAddress::isBetween(const MacAddress& begin, const MacAddress& end) const
{
    if (begin > end)
        return isBetween(end, begin);

    return *this > begin && *this < end;
}

bool MacAddress::isBroadcast(const MacAddress& mac) const
{
    return *this == broadcast();
}

std::string MacAddress::asString() const
{
    return fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
                       m_macAsBytes[5],
                       m_macAsBytes[4],
                       m_macAsBytes[3],
                       m_macAsBytes[2],
                       m_macAsBytes[1],
                       m_macAsBytes[0]);
}

std::array<std::uint8_t, 6> MacAddress::asBytes(ByteOrder::Value byteOrder) const
{
    return ByteOrder::copy(m_macAsBytes)
        .from(ByteOrder::NetworkOrder)
        .to(byteOrder);
}

std::uint64_t MacAddress::asInt() const
{
    std::uint64_t addr;
    memcpy(&addr, m_macAsBytes.data(), m_macAsBytes.size());
    return addr;
}

std::string MacAddress::format() const
{
    return asString();
}

} // namespace net
