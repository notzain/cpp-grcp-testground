#pragma once

#include "DeviceDiscoveryTask.h"

#include "core/net/scanner/ICMPScanner.h"
#include "core/traits/async/Resolver.h"

namespace net
{
class IcmpDeviceDiscoveryTask;

class IcmpPingResolver : public traits::FutureResolver<std::string, util::Result<net::ICMPResponse, net::IcmpError>>
{
    IcmpDeviceDiscoveryTask* m_discoveryTask;

  public:
    IcmpPingResolver(IcmpDeviceDiscoveryTask* discoveryTask);

    void onSuccess(const util::Result<net::ICMPResponse, net::IcmpError>& icmpResponse) override;
    void onException(const std::string& id, std::string_view error) override;
};

class IcmpDeviceDiscoveryTask : public DeviceDiscoveryTask
{

    IcmpPingResolver m_icmpResolver;
    net::ICMPScanner m_icmpScanner;

  public:
    IcmpDeviceDiscoveryTask(std::shared_ptr<ICMPSocket> socket);

    void start() override;

    void stop() override;

    void discover(std::string_view host) override;
};
} // namespace net