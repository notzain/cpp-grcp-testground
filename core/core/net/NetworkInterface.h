#pragma once

#include <PcapLiveDevice.h>
#include <optional>
#include <string>

#include "IPv4Address.h"
#include "MacAddress.h"
#include "core/util/Result.h"

namespace net
{
class NetworkInterface
{
    pcpp::PcapLiveDevice* device;

    NetworkInterface(pcpp::PcapLiveDevice* device);
    static std::optional<NetworkInterface> m_defaultInterface;

  public:
    static Result<NetworkInterface> byName(std::string_view interfaceName);
    static Result<NetworkInterface> byIp(std::string_view interfaceIp);
    static std::vector<NetworkInterface> availableInterfaces();
    static Result<NetworkInterface> defaultInterface();
    static void setDefaultInterface(const NetworkInterface& networkInterface);

    IPv4Address ipAddress() const;
    MacAddress macAddress() const;

    Result<IPv4Address> defaultGateway() const;
};
} // namespace net
