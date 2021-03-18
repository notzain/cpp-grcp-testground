#include "Device.h"

#include <fmt/format.h>

namespace core::models
{
Device::Device(std::string_view ip, std::string_view mac)
    : m_ipAddress(ip.data())
    , m_macAddress(mac.data())
{
}

pcpp::IPv4Address Device::ipAddress() const
{
    return m_ipAddress;
}

pcpp::MacAddress Device::macAddress() const
{
    return m_macAddress;
}

std::string Device::format() const
{
    return fmt::format("ip: {}, mac: {}", m_ipAddress.toString(), m_macAddress.toString());
}

nlohmann::json Device::serialize(nlohmann::json json) const
{
    json = {
        { "ipAddress", m_ipAddress.toString() },
        { "macAddress", m_macAddress.toString() }
    };
    return json;
}
} // namespace core::models