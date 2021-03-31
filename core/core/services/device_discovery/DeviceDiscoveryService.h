#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "DeviceDiscoveryTask.h"

namespace core::net
{
class DeviceDiscoveryService
{
    std::vector<std::unique_ptr<DeviceDiscoveryTask>> m_discoveryTasks;

  public:
    DeviceDiscoveryService() = default;

    template <typename T, typename... Args>
    void addDiscoveryTask(Args&&... args)
    {
        static_assert(std::is_base_of_v<DeviceDiscoveryTask, T>, "Type must be derived from DeviceDiscoveryTask");

        m_discoveryTasks.push_back(
            std::make_unique<T>(std::forward<Args>(args)...));

        m_discoveryTasks.back()->start();
    }

    void addDiscoveryTask(std::unique_ptr<DeviceDiscoveryTask> task);

    void discover(std::string_view host);
    void discover(std::string_view beginIp, std::string_view endIp);
};
} // namespace core::net