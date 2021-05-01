#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <string>

#include "core/util/Result.h"

namespace net
{
class DeviceDiscoveryTask
{
  public:
    struct TaskSuccess
    {
        std::string srcIp;
        std::string dstIp;
        std::uint8_t ttl;
        std::chrono::milliseconds responseTime;
        std::chrono::time_point<std::chrono::system_clock> completedAt;
    };

    struct TaskError
    {
        std::string srcIp;
        std::string dstIp;
        std::chrono::time_point<std::chrono::system_clock> completedAt;
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
    virtual void discover(std::string_view host) = 0;

    virtual void addSuccess(const std::string& host, TaskSuccess&& result);

    virtual void addError(const std::string& host, TaskError&& error);

    virtual bool hasResult(const std::string& host) const;

    virtual std::optional<ResultType> getResult(const std::string& host) const;

    virtual std::optional<ResultType> consumeResult(const std::string& host);

    virtual void clearResult(const std::string& host);

    virtual void clearResults();

    virtual std::function<void(const TaskSuccess&)>& onDevicePinged();

    virtual std::function<void(const TaskError&)>& onPingFailed();
};
} // namespace net