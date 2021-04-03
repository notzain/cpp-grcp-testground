#include "IcmpDeviceDiscoveryTask.h"
#include "core/util/logger/Logger.h"

#include <chrono>
namespace net
{
IcmpPingResolver::IcmpPingResolver(IcmpDeviceDiscoveryTask* discoveryTask)
    : FutureResolver(std::chrono::milliseconds(100))
    , m_discoveryTask(discoveryTask)
{
}

void IcmpPingResolver::onSuccess(const util::Result<net::ICMPResponse, net::IcmpError>& icmpResponse)
{
    if (icmpResponse)
    {
        m_discoveryTask->addSuccess(icmpResponse->dstIp.toString(),
                                    { icmpResponse->dstIp.toString(),
                                      icmpResponse->srcIp.toString(),
                                      icmpResponse->ttl,
                                      icmpResponse->responseTime,
                                      std::chrono::system_clock::now() });
    }
    else
    {
        const auto& error = icmpResponse.error();
        m_discoveryTask->addError(error.dstIp.toString(),
                                  { error.dstIp.toString(),
                                    error.srcIp.toString(),
                                    std::chrono::system_clock::now() });
    }
}

void IcmpPingResolver::onException(const std::string& id, std::string_view error)
{
    CORE_ERROR("'{}' exception: {}", id, error);
}

IcmpDeviceDiscoveryTask::IcmpDeviceDiscoveryTask(std::shared_ptr<ICMPSocket> socket)
    : m_icmpResolver(this)
    , m_icmpScanner(std::move(socket))
{
}

// IcmpDeviceDiscoveryTask::~IcmpDeviceDiscoveryTask()
// {
//     stop();
// }

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
} // namespace net