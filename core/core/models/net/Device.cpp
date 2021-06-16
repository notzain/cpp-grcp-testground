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

Milliseconds Device::responseTime() const
{
    return m_responseTime;
}

void Device::setResponseTime(const Milliseconds& ms)
{
    m_responseTime = ms;
}

const std::optional<net::SnmpVersion>& Device::lastUsedSnmpVersion() const
{
    return m_lastUsedSnmpVersion;
}

void Device::setLastUsedSnmpVersion(net::SnmpVersion version)
{
    m_lastUsedSnmpVersion = version;
}

const std::set<net::SnmpVersion>& Device::knownSnmpVersions() const
{
    return m_knownSnmpVersions;
}

void Device::addKnownSnmpVersion(net::SnmpVersion version)
{
    m_knownSnmpVersions.insert(version);
}

std::string Device::toString() const
{
    return fmt::format("ip: {}, mac: {}", m_ipAddress, m_macAddress);
}

nlohmann::json Device::serialize(nlohmann::json json) const
{
    json["ipAddress"] = m_ipAddress.toString();
    json["macAddress"] = m_macAddress.toString();
    json["responseTime"] = m_responseTime.count();

    if (m_lastUsedSnmpVersion)
        json["lastUsedSnmpVersion"] = *m_lastUsedSnmpVersion;
    else
        json["lastUsedSnmpVersion"] = nullptr;

    json["knownSnmpVersions"] = m_knownSnmpVersions;
    return json;
}
} // namespace models