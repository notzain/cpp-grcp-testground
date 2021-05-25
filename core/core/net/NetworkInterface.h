#pragma once

#include <PcapLiveDevice.h>
#include <memory>
#include <nonstd/expected.hpp>
#include <optional>
#include <string>

#include "IPv4Address.h"
#include "MacAddress.h"
#include "core/util/Result.h"

namespace net
{
class NetworkInterface : public std::enable_shared_from_this<NetworkInterface>
{
    pcpp::PcapLiveDevice* device;

    explicit NetworkInterface(pcpp::PcapLiveDevice* device);
    static std::optional<NetworkInterface> m_defaultInterface;

  public:
    static Result<NetworkInterface> byName(std::string_view interfaceName);
    static Result<NetworkInterface> byIp(std::string_view interfaceIp);
    static std::vector<pcpp::PcapLiveDevice*> availableInterfaces();
    static Result<NetworkInterface> defaultInterface();
    static void setDefaultInterface(const NetworkInterface& networkInterface);

    IPv4Address ipAddress() const;
    MacAddress macAddress() const;

    Result<IPv4Address> defaultGateway() const;
};
} // namespace net
