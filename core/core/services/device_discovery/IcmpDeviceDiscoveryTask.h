#pragma once

#include "DeviceDiscoveryTask.h"

#include "core/net/scanner/ICMPScanner.h"
#include "core/util/async/FutureResolver.h"

namespace net
{
class IcmpDeviceDiscoveryTask;

class IcmpPingResolver : public util::v2::FutureResolver<util::Result<net::ICMPResponse, net::IcmpError>>
{
    std::string m_host;
    IcmpDeviceDiscoveryTask* m_discoveryTask;

  public:
    IcmpPingResolver(std::string host, IcmpDeviceDiscoveryTask* discoveryTask)
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
    IcmpDeviceDiscoveryTask(std::shared_ptr<ICMPSocket> socket);

    void start() override;

    void stop() override;

    void discover(std::string_view host) override;
};
} // namespace net