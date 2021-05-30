#include "MyIcmpDeviceDiscoveryTask.h"
#include "core/net/MacAddress.h"

namespace app
{
MyIcmpDeviceDiscoveryTask::MyIcmpDeviceDiscoveryTask(net::v2::RawSocket::Ptr socket, repo::DeviceRepository& deviceRepo)
    : IcmpDeviceDiscoveryTask(socket)
    , m_deviceRepo(deviceRepo)
{
}

void MyIcmpDeviceDiscoveryTask::discover(const net::IPv4Address& host)
{
    if (auto possibleDevice = m_deviceRepo.find(host))
        postRequest(createPingResolver(host, possibleDevice.value()->macAddress()));
    else
        postRequest(createPingResolver(host, net::MacAddress::broadcast()));
}
} // namespace app
