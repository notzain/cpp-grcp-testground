#include "IcmpDeviceDiscoveryTask.h"

#include <chrono>
#include <spdlog/fmt/chrono.h>

namespace core::net
{
IcmpPingResolver::IcmpPingResolver(IcmpDeviceDiscoveryTask* discoveryTask)
    : FutureResolver(std::chrono::milliseconds(100))
    , m_discoveryTask(discoveryTask)
{
}

void IcmpPingResolver::onSuccess(const net::ICMPResponse& icmpResponse)
{
    m_discoveryTask->addSuccess(icmpResponse.dstIp.toString(),
                                { icmpResponse.dstIp.toString(),
                                  icmpResponse.srcIp.toString(),
                                  icmpResponse.ttl,
                                  icmpResponse.responseTime,
                                  std::chrono::system_clock::now() });
}

void IcmpPingResolver::onFailure(const std::string& id, std::string_view error)
{
    m_discoveryTask->addError(id, { id });
}

IcmpDeviceDiscoveryTask::IcmpDeviceDiscoveryTask(std::shared_ptr<ICMPSocket> socket)
    : m_icmpResolver(this)
    , m_icmpScanner(std::move(socket))
{
}

void IcmpDeviceDiscoveryTask::start()
{
    m_icmpScanner.start();
    m_icmpResolver.start();
}

void IcmpDeviceDiscoveryTask::stop()
{
    m_icmpScanner.stop();
    m_icmpResolver.stop();
}

void IcmpDeviceDiscoveryTask::discover(std::string_view host)
{
    m_icmpResolver.enqueue(host.data(), m_icmpScanner.pingAsync(host));
}
} // namespace core::net