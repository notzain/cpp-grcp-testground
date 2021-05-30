#include "Device.h"

#include <fmt/format.h>

namespace models
{
Device::Device(net::IPv4Address ip, net::MacAddress mac)
    : m_ipAddress(std::move(ip))
    , m_macAddress(std::move(mac))
{
}

net::IPv4Address Device::ipAddress() const
{
    return m_ipAddress;
}

net::MacAddress Device::macAddress() const
{
    return m_macAddress;
}

std::string Device::format() const
{
    return fmt::format("ip: {}, mac: {}", m_ipAddress, m_macAddress);
}

nlohmann::json Device::serialize(nlohmann::json json) const
{
    json = {
        { "ipAddress", m_ipAddress.asString() },
        { "macAddress", m_macAddress.asString() }
    };
    return json;
}
} // namespace models