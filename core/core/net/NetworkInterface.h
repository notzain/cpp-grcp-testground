#pragma once

#include <PcapFileDevice.h>
#include <PcapLiveDevice.h>
#include <nonstd/expected.hpp>
#include <string>

namespace core::net
{
class NetworkInterface
{
  public:
    static nonstd::expected<NetworkInterface, std::string> GetByName(const std::string_view interfaceName);
    static nonstd::expected<NetworkInterface, std::string> GetByIp(const std::string_view interfaceIp);
    static std::vector<pcpp::PcapLiveDevice*> GetAvailableInterfaces();

    pcpp::PcapLiveDevice *device;

  private:
    NetworkInterface(pcpp::PcapLiveDevice *device);
};
} // namespace core::net
