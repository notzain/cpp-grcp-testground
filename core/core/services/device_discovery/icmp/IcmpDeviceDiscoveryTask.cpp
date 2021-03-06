#include "IcmpDeviceDiscoveryTask.h"
#include "core/net/MacAddress.h"
#include "core/net/NetworkInterface.h"
#include "core/util/Time.h"
#include "core/util/async/Task.h"
#include "core/util/logger/Logger.h"

#include <chrono>
#include <exception>
namespace net
{
void IcmpPingResolver::onCompletion(const Result<IcmpResponse, IcmpError>& icmpResponse)
{
    if (icmpResponse)
    {
        // Reverse src and dst so we follow the "request" order, and not the "reply" order
        m_discoveryTask->addSuccess(icmpResponse->dstIp,
                                    { icmpResponse->dstIp,
                                      icmpResponse->dstMac,
                                      icmpResponse->srcIp,
                                      icmpResponse->srcMac,
                                      icmpResponse->ttl,
                                      icmpResponse->responseTime,
                                      SystemClock::now() });
    }
    else
    {
        const auto& error = icmpResponse.error();
        m_discoveryTask->addError(error.dstIp,
                                  { error.dstIp,
                                    SystemClock::now() });
    }
}

void IcmpPingResolver::onException(const std::exception_ptr& e)
{
    try
    {
        if (e)
        {
            std::rethrow_exception(e);
        }
    }
    catch (const std::exception& e)
    {
        CORE_ERROR("{}: {}", m_host, e.what());
    }
}

IcmpDeviceDiscoveryTask::IcmpDeviceDiscoveryTask(v2::RawSocket::Ptr socket)
    : m_icmpScanner(*NetworkInterface::byName("enp0s3"), std::move(socket))
{
}

// IcmpDeviceDiscoveryTask::~IcmpDeviceDiscoveryTask()
// {
//     stop();
// }

void IcmpDeviceDiscoveryTask::start()
{
    m_icmpScanner.startScanning();
}

void IcmpDeviceDiscoveryTask::stop()
{
    m_icmpScanner.stopScanning();
}

void IcmpDeviceDiscoveryTask::discover(const IPv4Address& host)
{
    auto resolver = createPingResolver(host, MacAddress::broadcast());
    postRequest(resolver);
}

std::shared_ptr<IcmpPingResolver> IcmpDeviceDiscoveryTask::createPingResolver(const IPv4Address& ip, const MacAddress& mac)
{
    auto resolver = std::make_shared<IcmpPingResolver>(ip, this);
    resolver->setFuture(m_icmpScanner.pingAsync(ip, mac));
    return resolver;
}

void IcmpDeviceDiscoveryTask::postRequest(std::shared_ptr<IcmpPingResolver> resolver)
{
    util::TaskRunner::defaultRunner()
        .post(resolver);
}
} // namespace net