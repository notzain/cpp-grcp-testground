#pragma once

#include "DeviceDiscoveryTask.h"

#include "core/net/IPv4Address.h"
#include "core/net/scanner/ICMPScanner.h"
#include "core/util/async/FutureResolver.h"

namespace net
{
class IcmpDeviceDiscoveryTask;

class IcmpPingResolver : public util::v2::FutureResolver<util::Result<net::ICMPResponse, net::IcmpError>>
{
    IPv4Address m_host;
    IcmpDeviceDiscoveryTask* m_discoveryTask;

  public:
    IcmpPingResolver(IPv4Address host, IcmpDeviceDiscoveryTask* discoveryTask)
        : m_host(std::move(host))
        , m_discoveryTask(discoveryTask)
    {
    }

    void onCompletion(const util::Result<net::ICMPResponse, net::IcmpError>& icmpResponse) override;
    void onException(const std::exception_ptr& exception) override;
};

class IcmpDeviceDiscoveryTask : public DeviceDiscoveryTask
{
    net::ICMPScanner m_icmpScanner;

  public:
    IcmpDeviceDiscoveryTask(v2::RawSocket::Ptr socket);

    void start() override;

    void stop() override;

    void discover(const IPv4Address& host) override;
};
} // namespace net