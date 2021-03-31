#pragma once

#include "DeviceDiscoveryTask.h"

#include "core/net/scanner/ICMPScanner.h"
#include "core/traits/async/Resolver.h"

namespace core::net
{
class IcmpDeviceDiscoveryTask;

class IcmpPingResolver : public traits::FutureResolver<std::string, net::ICMPResponse>
{
    IcmpDeviceDiscoveryTask* m_discoveryTask;

  public:
    IcmpPingResolver(IcmpDeviceDiscoveryTask* discoveryTask);

    void onSuccess(const net::ICMPResponse& icmpResponse) override;
    void onFailure(const std::string& id, std::string_view error) override;
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
} // namespace core::net