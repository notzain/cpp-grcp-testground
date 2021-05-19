#include "IcmpDeviceDiscoveryTask.h"
#include "core/util/Time.h"
#include "core/util/async/Task.h"
#include "core/util/logger/Logger.h"

#include <chrono>
#include <exception>
namespace net
{
void IcmpPingResolver::onCompletion(const util::Result<net::ICMPResponse, net::IcmpError>& icmpResponse)
{
    if (icmpResponse)
    {
        m_discoveryTask->addSuccess(icmpResponse->dstIp,
                                    { icmpResponse->dstIp,
                                      icmpResponse->srcIp,
                                      icmpResponse->ttl,
                                      icmpResponse->responseTime,
                                      SystemClock::now()  });
    }
    else
    {
        const auto& error = icmpResponse.error();
        m_discoveryTask->addError(error.dstIp,
                                  { error.dstIp,
                                    error.srcIp,
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
    : m_icmpScanner(std::move(socket))
{
}

// IcmpDeviceDiscoveryTask::~IcmpDeviceDiscoveryTask()
// {
//     stop();
// }

void IcmpDeviceDiscoveryTask::start()
{
    m_icmpScanner.start();
}

void IcmpDeviceDiscoveryTask::stop()
{
    m_icmpScanner.stop();
}

void IcmpDeviceDiscoveryTask::discover(const IPv4Address& host)
{
    auto resolver = std::make_shared<IcmpPingResolver>(host, this);
    resolver->setFuture(m_icmpScanner.pingAsync(host));

    util::TaskRunner::defaultRunner()
        .post(resolver);
}
} // namespace net