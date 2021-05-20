#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <string>

#include "core/net/IPv4Address.h"
#include "core/net/MacAddress.h"
#include "core/util/Result.h"
#include "core/util/Time.h"

namespace net
{
class DeviceDiscoveryTask
{
  public:
    struct TaskSuccess
    {
        IPv4Address srcIp;
        std::optional<MacAddress> srcMac;

        IPv4Address dstIp;
        std::optional<MacAddress> dstMac;

        std::uint8_t ttl;
        Milliseconds responseTime;
        TimePoint completedAt;
    };

    struct TaskError
    {
        IPv4Address srcIp;
        IPv4Address dstIp;
        TimePoint completedAt;
    };

    using ResultType = util::Result<TaskSuccess, TaskError>;

  private:
    std::map<std::string, ResultType> m_results;
    std::function<void(const TaskSuccess&)> m_onDevicePinged;
    std::function<void(const TaskError&)> m_onPingFailed;

  public:
    virtual ~DeviceDiscoveryTask() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void discover(const IPv4Address& host) = 0;

    virtual void addSuccess(const IPv4Address& host, TaskSuccess&& result);

    virtual void addError(const IPv4Address& host, TaskError&& error);

    virtual bool hasResult(const IPv4Address& host) const;

    virtual std::optional<ResultType> getResult(const IPv4Address& host) const;

    virtual std::optional<ResultType> consumeResult(const IPv4Address& host);

    virtual void clearResult(const IPv4Address& host);

    virtual void clearResults();

    virtual std::function<void(const TaskSuccess&)>& onDevicePinged();

    virtual std::function<void(const TaskError&)>& onPingFailed();
};
} // namespace net