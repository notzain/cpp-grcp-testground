#include "DeviceDiscoveryTask.h"
#include "core/net/IPv4Address.h"

namespace net
{
void DeviceDiscoveryTask::addSuccess(const IPv4Address& host, TaskSuccess&& result)
{
    if (m_onDevicePinged)
        m_onDevicePinged(result);

    m_results[host.toString()] = std::move(result);
}

void DeviceDiscoveryTask::addError(const IPv4Address& host, TaskError&& error)
{
    if (m_onPingFailed)
        m_onPingFailed(error);

    m_results[host.toString()] = Error(std::move(error));
}

bool DeviceDiscoveryTask::hasResult(const IPv4Address& host) const
{
    return m_results.count(host.toString());
}

std::optional<DeviceDiscoveryTask::ResultType> DeviceDiscoveryTask::getResult(const IPv4Address& host) const
{
    if (hasResult(host))
    {
        return m_results.at(host.toString());
    }
    return std::nullopt;
}

std::optional<DeviceDiscoveryTask::ResultType> DeviceDiscoveryTask::consumeResult(const IPv4Address& host)
{
    auto result = getResult(host);
    if (result)
    {
        clearResult(host);
    }
    return result;
}

void DeviceDiscoveryTask::clearResults()
{
    m_results.clear();
}

void DeviceDiscoveryTask::clearResult(const IPv4Address& host)
{
    m_results.erase(host.toString());
}

std::function<void(const DeviceDiscoveryTask::TaskError&)>& DeviceDiscoveryTask::onPingFailed()
{
    return m_onPingFailed;
}

std::function<void(const DeviceDiscoveryTask::TaskSuccess&)>& DeviceDiscoveryTask::onDevicePinged()
{
    return m_onDevicePinged;
}

} // namespace net