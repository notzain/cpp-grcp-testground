#include "NetworkInterface.h"
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>

#include <spdlog/fmt/fmt.h>

namespace net
{
std::optional<NetworkInterface> NetworkInterface::m_defaultInterface = std::nullopt;

NetworkInterface::NetworkInterface(pcpp::PcapLiveDevice* device)
    : device(device)
{
}

nonstd::expected<NetworkInterface, std::string> NetworkInterface::byName(const std::string_view interfaceName)
{
    if (auto* device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interfaceName.data()))
    {
        return NetworkInterface(device);
    }
    else
    {
        return nonstd::make_unexpected(fmt::format("Could not find interface '{}'", interfaceName));
    }
}

nonstd::expected<NetworkInterface, std::string> NetworkInterface::byIp(const std::string_view interfaceIp)
{
    if (auto* device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIp.data()))
    {
        return NetworkInterface(device);
    }
    else
    {
        return nonstd::make_unexpected(fmt::format("Could not find interface '{}'", interfaceIp));
    }
}

std::vector<pcpp::PcapLiveDevice*> NetworkInterface::availableInterfaces()
{
    return pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
}

util::Result<NetworkInterface> NetworkInterface::defaultInterface()
{
    if (m_defaultInterface.has_value())
        return *m_defaultInterface;
    else
        return util::Result<NetworkInterface>::unexpected("No default interface was set.");
}

void NetworkInterface::setDefaultInterface(const NetworkInterface& networkInterface)
{
    m_defaultInterface = networkInterface;
}
} // namespace net
