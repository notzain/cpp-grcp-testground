#pragma once

#include "core/net/MacAddress.h"
#include "core/services/device_discovery/DeviceDiscoveryTask.h"

#include "core/net/IPv4Address.h"
#include "core/net/MacAddress.h"
#include "core/net/scanner/IcmpScanner.h"
#include "core/util/async/FutureResolver.h"

namespace net
{
class IcmpDeviceDiscoveryTask;

class IcmpPingResolver : public util::v2::FutureResolver<Result<IcmpResponse, IcmpError>>
{
    IPv4Address m_host;
    IcmpDeviceDiscoveryTask* m_discoveryTask;

  public:
    IcmpPingResolver(IPv4Address host, IcmpDeviceDiscoveryTask* discoveryTask)
        : m_host(std::move(host))
        , m_discoveryTask(discoveryTask)
    {
    }

    void onCompletion(const Result<IcmpResponse, IcmpError>& icmpResponse) override;
    void onException(const std::exception_ptr& exception) override;
};

class IcmpDeviceDiscoveryTask : public DeviceDiscoveryTask
{
    net::IcmpScanner m_icmpScanner;

  public:
    IcmpDeviceDiscoveryTask(v2::RawSocket::Ptr socket);

    void start() override;

    void stop() override;

    void discover(const IPv4Address& host) override;

  protected:
    std::shared_ptr<IcmpPingResolver> createPingResolver(const IPv4Address& ip, const MacAddress& mac);
    void postRequest(std::shared_ptr<IcmpPingResolver> resolver);
};
} // namespace net