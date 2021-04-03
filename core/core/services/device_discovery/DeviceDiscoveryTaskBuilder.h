#pragma once

#include "DeviceDiscoveryTask.h"

namespace net
{
class DeviceDiscoveryTaskBuilder
{
    std::function<void(const DeviceDiscoveryTask::Success&)> m_onDevicePinged;
    std::function<void(const DeviceDiscoveryTask::Error&)> m_onPingFailed;
    std::unique_ptr<DeviceDiscoveryTask> m_task;

  public:
    DeviceDiscoveryTaskBuilder() = default;

    template <typename T, typename... Args>
    DeviceDiscoveryTaskBuilder& construct(Args&&... args)
    {
        static_assert(std::is_base_of_v<DeviceDiscoveryTask, T>, "Type must be derived from DeviceDiscoveryTask");
        m_task = std::make_unique<T>(std::forward<Args>(args)...);

        return *this;
    }

    DeviceDiscoveryTaskBuilder& withSuccessCallback(std::function<void(const DeviceDiscoveryTask::Success&)>&& func)
    {
        m_onDevicePinged = std::move(func);

        return *this;
    }

    DeviceDiscoveryTaskBuilder& withFailureCallback(std::function<void(const DeviceDiscoveryTask::Error&)>&& func)
    {
        m_onPingFailed = std::move(func);

        return *this;
    }

    std::unique_ptr<DeviceDiscoveryTask> build()
    {
        if (m_task)
        {
            m_task->onDevicePinged() = std::move(m_onDevicePinged);
            m_task->onPingFailed() = std::move(m_onPingFailed);
        }

        return std::move(m_task);
    }

    operator std::unique_ptr<DeviceDiscoveryTask>()
    {
        return build();
    }
};
} // namespace net