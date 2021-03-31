#include "DeviceDiscoveryTask.h"

namespace core::net
{
void DeviceDiscoveryTask::addSuccess(const std::string& host, Success&& result)
{
    if (m_onDevicePinged)
        m_onDevicePinged(result);

    m_results[host] = std::move(result);
}

void DeviceDiscoveryTask::addError(const std::string& host, Error&& error)
{
    if (m_onPingFailed)
        m_onPingFailed(error);

    m_results[host] = ResultType::unexpected(std::move(error));
}

bool DeviceDiscoveryTask::hasResult(const std::string& host) const
{
    return m_results.count(host);
}

std::optional<DeviceDiscoveryTask::ResultType> DeviceDiscoveryTask::getResult(const std::string& host) const
{
    if (hasResult(host))
    {
        return m_results.at(host);
    }
    return std::nullopt;
}

std::optional<DeviceDiscoveryTask::ResultType> DeviceDiscoveryTask::consumeResult(const std::string& host)
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

void DeviceDiscoveryTask::clearResult(const std::string& host)
{
    m_results.erase(host);
}

std::function<void(const DeviceDiscoveryTask::Error&)>& DeviceDiscoveryTask::onPingFailed()
{
    return m_onPingFailed;
}

std::function<void(const DeviceDiscoveryTask::Success&)>& DeviceDiscoveryTask::onDevicePinged()
{
    return m_onDevicePinged;
}

} // namespace core::net