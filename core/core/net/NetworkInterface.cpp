#include "NetworkInterface.h"
#include "core/net/MacAddress.h"
#include "core/util/Result.h"
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

Result<NetworkInterface> NetworkInterface::byName(const std::string_view interfaceName)
{
    if (auto* device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(interfaceName.data()))
    {
        return NetworkInterface(device);
    }
    else
    {
        return Error(ErrorType::NotFound);
    }
}

Result<NetworkInterface> NetworkInterface::byIp(const std::string_view interfaceIp)
{
    if (auto* device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIp.data()))
    {
        return NetworkInterface(device);
    }
    else
    {
        return Error(ErrorType::NotFound);
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
        return Error(ErrorType::NotFound);
}

void NetworkInterface::setDefaultInterface(const NetworkInterface& networkInterface)
{
    m_defaultInterface = networkInterface;
}

IPv4Address NetworkInterface::ipAddress() const
{
    return device->getIPv4Address();
}

MacAddress NetworkInterface::macAddress() const
{
    return device->getMacAddress();
}

Result<IPv4Address> NetworkInterface::defaultGateway() const
{
    auto gateway = IPv4Address(device->getDefaultGateway());
    if (gateway == IPv4Address::zero())
        return Error(ErrorType::Unknown);
    return gateway;
}
} // namespace net
