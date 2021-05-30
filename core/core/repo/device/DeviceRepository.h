#pragma once

#include "core/models/net/Device.h"
#include "core/net/IPv4Address.h"
#include "core/net/MacAddress.h"
#include "core/repo/Repository.h"
#include "core/util/IterResult.h"
#include "core/util/Result.h"

#include <algorithm>
#include <map>
#include <memory>

namespace repo
{
class DeviceRepository : public Repository<std::shared_ptr<models::Device>, net::MacAddress>
{
    using DevicePtr = std::shared_ptr<models::Device>;
    std::map<net::MacAddress, DevicePtr> m_devices;

  public:
    using Ptr = std::shared_ptr<DeviceRepository>;

    DeviceRepository() = default;

    bool create(const net::MacAddress& mac, DevicePtr&& item) override;
    Result<DevicePtr> read(const net::MacAddress& mac) const override;
    bool update(const net::MacAddress& mac, DevicePtr&& item) override;
    bool remove(const net::MacAddress& mac) override;
    bool has(const net::MacAddress& mac) const override;

    Result<DevicePtr> find(const net::IPv4Address& ip) const;
    Result<DevicePtr> find(const net::MacAddress& mac) const;

    template <typename Func>
    Result<DevicePtr> findBy(Func&& func) const
    {
        for (const auto& [_, device] : m_devices)
        {
            if (func(*device))
                return device;
        }
        return Error(ErrorType::NotFound);
    }

    template <typename Func>
    void iterate(Func&& func)
    {
        for (auto& [_, device] : m_devices)
        {
            if (func(*device) == IterResult::Break)
                break;
        }
    }

    template <typename Func>
    bool contains(Func&& func) const
    {
        return std::any_of(m_devices.begin(), m_devices.end(), [&func](const auto& it) { return func(*it->second); });
    }
};
} // namespace repo