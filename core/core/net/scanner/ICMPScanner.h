#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string_view>

#include "IScanner.h"
#include "IpAddress.h"
#include "MacAddress.h"
#include "core/net/scanner/IScanner.h"
#include "core/net/socket/ICMPSocket.h"
#include "core/util/Result.h"
#include "core/util/type/WrapAround.h"

namespace net
{

struct ICMPResponse
{
    pcpp::IPv4Address srcIp;
    pcpp::IPv4Address dstIp;

    std::uint8_t ttl;
    std::chrono::milliseconds responseTime;
};

struct IcmpError
{
    pcpp::IPv4Address srcIp;
    pcpp::IPv4Address dstIp;
    enum Error
    {
        Timeout
    } error;
};

class ICMPScanner : public IAsyncScanner<ICMPResponse, IcmpError>
{
    std::shared_ptr<ICMPSocket> m_socket;
    util::WrapAround<std::uint8_t> m_sequenceNumber;

    struct Request
    {
        std::string dstIp;
        std::promise<util::Result<ICMPResponse, IcmpError>> promise;
        std::chrono::time_point<std::chrono::system_clock> time;
    };
    std::map<std::string, Request> m_pendingRequests;

  public:
    ICMPScanner(std::shared_ptr<ICMPSocket> socket);

    util::Result<ICMPResponse, IcmpError> ping(std::string_view host) override;
    std::future<util::Result<ICMPResponse, IcmpError>> pingAsync(std::string_view host) override;

  private:
    void onPacketReceived(std::uint8_t* bytes, std::size_t len) override;
    void handleTimeouts() override;
};
} // namespace net
