#include "MacAddress.h"
#include <cstdint>

namespace net
{
MacAddress::MacAddress(std::array<std::uint8_t, 6> bytes)
    : m_macAsBytes(std::move(bytes))
{
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

Result<MacAddress> MacAddress::parse(std::string_view ip)
{
    // FIXME: No input sanitizing, insecure sscanf. Should do better.
    std::array<std::uint8_t, 6> bytes;
    int n = 0;
    if (std::sscanf(ip.data(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx%n", &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5], &n) == 6 &&
        ip[n] == '\0')
    {
        return MacAddress(ByteOrder::copy(bytes)
                              .from(ByteOrder::HostOrder)
                              .to(ByteOrder::NetworkOrder));
    }

    return Error(ErrorType::InvalidArgument);
}

Result<MacAddress> MacAddress::parse(std::uint64_t number)
{
    return Error(ErrorType::Unimplemented);
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

std::string MacAddress::asString() const
{
    return fmt::format("{:X}:{:X}:{:X}:{:X}:{:X}:{:X}",
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
    return 0;
}

std::string MacAddress::format() const
{
    return asString();
}

} // namespace net
