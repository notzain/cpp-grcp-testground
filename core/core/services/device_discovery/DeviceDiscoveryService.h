#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include <spdlog/fmt/chrono.h>

#include "core/net/IPv4Range.h"
#include "core/net/scanner/ICMPScanner.h"
#include "core/traits/async/Resolver.h"

namespace core::net
{
class DeviceDiscoveryTask
{
  public:
    virtual ~DeviceDiscoveryTask() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void discover(std::string_view host) = 0;
};

class IcmpDeviceDiscoveryTask : public DeviceDiscoveryTask
{
    class IcmpPingResolver : public traits::FutureResolver<net::ICMPResponse>
    {
        IcmpDeviceDiscoveryTask* m_discoveryTask;

      public:
        IcmpPingResolver(IcmpDeviceDiscoveryTask* discoveryTask)
            : FutureResolver(std::chrono::milliseconds(100))
            , m_discoveryTask(discoveryTask)
        {
        }

        void onNextItem(const net::ICMPResponse& icmpResponse) override
        {
            CORE_INFO("Reply from '{}' to '{}' took {}",
                      icmpResponse.srcIp.toString(),
                      icmpResponse.dstIp.toString(),
                      icmpResponse.responseTime);
        }
    } m_icmpResolver;

    net::ICMPScanner m_icmpScanner;

  public:
    IcmpDeviceDiscoveryTask(std::shared_ptr<ICMPSocket> socket)
        : m_icmpResolver(this)
        , m_icmpScanner(std::move(socket))
    {
    }

    void start() override
    {
        m_icmpResolver.start();
        m_icmpScanner.start();
    }

    void stop() override
    {
        m_icmpResolver.stop();
        m_icmpScanner.stop();
    }

    void discover(std::string_view host) override
    {
        m_icmpResolver.enqueue(m_icmpScanner.pingAsync(host));
    }
};

class DeviceDiscoveryService
{
    std::vector<std::unique_ptr<DeviceDiscoveryTask>> m_discoveryTasks;

  public:
    template <typename T, typename... Args>
    void addDiscoveryTask(Args&&... args)
    {
        static_assert(std::is_base_of_v<DeviceDiscoveryTask, T>, "Type must be derived from DeviceDiscoveryTask");

        m_discoveryTasks.push_back(
            std::make_unique<T>(std::forward<Args>(args)...));
        m_discoveryTasks.back()->start();
    }

    void runOnce(std::string_view beginIp, std::string_view endIp)
    {
        for (const auto ip : util::rangeOf<pcpp::IPv4Address>({ beginIp.data() }, { endIp.data() }))
        {
            discover(ip.toString());
        }
    }

  private:
    void discover(std::string_view host)
    {
        for (auto& task : m_discoveryTasks)
        {
            task->discover(host);
        }
    }
};
} // namespace core::net