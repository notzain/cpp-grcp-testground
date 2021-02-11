#include "NetworkInterface.h"
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <fmt/format.h>
#include <nonstd/expected.hpp>

namespace core::net
{
nonstd::expected<NetworkInterface, std::string> NetworkInterface::GetByName(const std::string_view interfaceName)
{
    if (auto *device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interfaceName.data()))
    {
        return NetworkInterface(device);
    }
    else
    {
        return nonstd::make_unexpected(fmt::format("Could not find interface '{}'", interfaceName));
    }
}

nonstd::expected<NetworkInterface, std::string> NetworkInterface::GetByIp(const std::string_view interfaceIp)
{
    if (auto *device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIp.data()))
    {
        return NetworkInterface(device);
    }
    else
    {
        return nonstd::make_unexpected(fmt::format("Could not find interface '{}'", interfaceIp));
    }
}

std::vector<pcpp::PcapLiveDevice *> NetworkInterface::GetAvailableInterfaces()
{
    return pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
}

NetworkInterface::NetworkInterface(pcpp::PcapLiveDevice *device)
    : device(device)
{
}
} // namespace core::net
