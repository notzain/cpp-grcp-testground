#pragma once

#include <PcapLiveDevice.h>
#include <memory>
#include <nonstd/expected.hpp>
#include <optional>
#include <string>

#include "core/util/Result.h"

namespace net
{
class NetworkInterface : public std::enable_shared_from_this<NetworkInterface>
{
    explicit NetworkInterface(pcpp::PcapLiveDevice* device);

    static std::optional<NetworkInterface> m_defaultInterface;

  public:
    static nonstd::expected<NetworkInterface, std::string> byName(std::string_view interfaceName);
    static nonstd::expected<NetworkInterface, std::string> byIp(std::string_view interfaceIp);
    static std::vector<pcpp::PcapLiveDevice*> availableInterfaces();
    static util::Result<NetworkInterface> defaultInterface();
    static void setDefaultInterface(const NetworkInterface& networkInterface);

    pcpp::PcapLiveDevice* device;
};
} // namespace net
