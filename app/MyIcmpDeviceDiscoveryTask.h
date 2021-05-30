#pragma once

#include <core/net/socket/v2/RawSocket.h>
#include <core/repo/device/DeviceRepository.h>
#include <core/services/device_discovery/icmp/IcmpDeviceDiscoveryTask.h>

namespace app
{
class MyIcmpDeviceDiscoveryTask : public net::IcmpDeviceDiscoveryTask
{
    repo::DeviceRepository& m_deviceRepo;

  public:
    MyIcmpDeviceDiscoveryTask(net::v2::RawSocket::Ptr socket, repo::DeviceRepository& deviceRepo);

    void discover(const net::IPv4Address& host) override;
};
} // namespace app