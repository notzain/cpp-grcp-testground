#include "DeviceDiscoveryService.h"
#include "core/net/IPv4Address.h"
#include "core/util/Sequence.h"

namespace net
{
void DeviceDiscoveryService::discover(const IPv4Address& begin, const IPv4Address& end)
{
    for (const auto ip : util::rangeOf(begin, util::Inclusive(end)))
    {
        discover(ip);
    }
}

void DeviceDiscoveryService::discover(const IPv4Address& host)
{
    for (auto& task : m_discoveryTasks)
    {
        task->discover(host);
    }
}

void DeviceDiscoveryService::addDiscoveryTask(std::unique_ptr<DeviceDiscoveryTask> task)
{
    m_discoveryTasks.push_back(std::move(task));
    m_discoveryTasks.back()->start();
}

void DeviceDiscoveryService::clearResults()
{
    for (auto& task : m_discoveryTasks)
    {
        task->clearResults();
    }
}

} // namespace net