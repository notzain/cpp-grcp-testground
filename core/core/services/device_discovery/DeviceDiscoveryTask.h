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
    struct Success
    {
        std::string srcIp;
        std::string dstIp;
        std::uint8_t ttl;
        std::chrono::milliseconds responseTime;
        std::chrono::time_point<std::chrono::system_clock> completedAt;
    };

    struct Error
    {
        std::string srcIp;
        std::string dstIp;
        std::chrono::time_point<std::chrono::system_clock> completedAt;
    };

    using ResultType = util::Result<Success, Error>;

  private:
    std::map<std::string, ResultType> m_results;
    std::function<void(const Success&)> m_onDevicePinged;
    std::function<void(const Error&)> m_onPingFailed;

  public:
    virtual ~DeviceDiscoveryTask() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void discover(std::string_view host) = 0;

    virtual void addSuccess(const std::string& host, Success&& result);

    virtual void addError(const std::string& host, Error&& error);

    virtual bool hasResult(const std::string& host) const;

    virtual std::optional<ResultType> getResult(const std::string& host) const;

    virtual std::optional<ResultType> consumeResult(const std::string& host);

    virtual void clearResult(const std::string& host);

    virtual void clearResults();

    virtual std::function<void(const Success&)>& onDevicePinged();

    virtual std::function<void(const Error&)>& onPingFailed();
};
} // namespace net