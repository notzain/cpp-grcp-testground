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
#include "core/util/type/WrapAround.h"

namespace core::net
{

struct ICMPResponse
{
    pcpp::IPv4Address srcIp;
    pcpp::IPv4Address dstIp;

    std::uint8_t ttl;
    std::chrono::milliseconds responseTime;
};

class ICMPScanner : public IAsyncScanner<ICMPResponse>
{
    std::shared_ptr<ICMPSocket> m_socket;
    util::WrapAround<std::uint8_t> m_sequenceNumber;

    struct Request
    {
        std::promise<ICMPResponse> promise;
        std::chrono::time_point<std::chrono::system_clock> time;
    };
    std::map<std::string, Request> m_pendingRequests;

  public:
    ICMPScanner(std::shared_ptr<ICMPSocket> socket);
    virtual ~ICMPScanner() = default;

    util::Result<ICMPResponse> ping(std::string_view host) override;
    std::future<ICMPResponse> pingAsync(std::string_view host) override;

  private:
    void onPacketReceived(std::uint8_t* bytes, std::size_t len) override;
    void handleTimeouts() override;
};
} // namespace core::net
