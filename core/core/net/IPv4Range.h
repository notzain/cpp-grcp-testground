#pragma once

#include "core/util/Sequence.h"

#include <IpAddress.h>

#include <array>
#include <cstdint>

namespace util
{
template <>
[[nodiscard]] inline pcpp::IPv4Address Sequence<pcpp::IPv4Address>::increment(const pcpp::IPv4Address& address) const
{
    const auto* currentIp = address.toBytes();
    std::array<std::uint8_t, 4> bytes = { currentIp[0], currentIp[1], currentIp[2], currentIp[3] };
    for (int i = static_cast<int>(bytes.size()) - 1; i >= 0; --i)
    {
        if (bytes[i] == static_cast<std::uint8_t>(255))
        {
            bytes[i] = static_cast<std::uint8_t>(0);
        }
        else
        {
            bytes[i]++;
            break;
        }
    }

    return pcpp::IPv4Address(bytes.data());
}
} // namespace util
