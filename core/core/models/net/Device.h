#pragma once

#include "core/net/IPv4Address.h"
#include "core/net/MacAddress.h"
#include "core/net/snmp/SnmpDefs.h"
#include "core/traits/Printable.h"
#include "core/traits/serializer/JsonSerializer.h"
#include "core/util/Time.h"
#include <set>

namespace models
{
class Device
    : public traits::Printable<Device>
    , public traits::JsonSerializer
{
    net::IPv4Address m_ipAddress;
    net::MacAddress m_macAddress;
    Milliseconds m_responseTime;

    std::optional<net::SnmpVersion> m_lastUsedSnmpVersion;
    std::set<net::SnmpVersion> m_knownSnmpVersions;

  public:
    Device(net::IPv4Address ip, net::MacAddress mac);
    virtual ~Device() = default;

    net::IPv4Address ipAddress() const;
    net::MacAddress macAddress() const;

    Milliseconds responseTime() const;
    void setResponseTime(const Milliseconds& ms);

    const std::optional<net::SnmpVersion>& lastUsedSnmpVersion() const;
    void setLastUsedSnmpVersion(net::SnmpVersion version);

    const std::set<net::SnmpVersion>& knownSnmpVersions() const;
    void addKnownSnmpVersion(net::SnmpVersion version);

    std::string toString() const override;
    nlohmann::json serialize(nlohmann::json json = {}) const override;
};
} // namespace models