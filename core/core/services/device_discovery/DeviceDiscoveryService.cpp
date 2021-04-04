#include "DeviceDiscoveryService.h"
#include "core/net/IPv4Range.h"

namespace net
{
void DeviceDiscoveryService::discover(std::string_view beginIp, std::string_view endIp)
{
    for (const auto ip : util::rangeOf<pcpp::IPv4Address>({ beginIp.data() }, { endIp.data() }))
    {
        discover(ip.toString());
    }
}

void DeviceDiscoveryService::discover(std::string_view host)
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