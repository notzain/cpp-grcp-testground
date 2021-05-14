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
#include "core/net/socket/v2/IcmpSocket.h"
#include "core/net/socket/v2/Protocols.h"
#include "core/net/socket/v2/RawSocket.h"
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
    ErrorType error;
};

class ICMPScanner : public IAsyncScanner<v2::IcmpProtocol, ICMPResponse, IcmpError>
{
    v2::RawSocket::Ptr m_socket;
    // v2::RawSocket::Ptr m_rawSocket;
    util::WrapAround<std::uint8_t> m_sequenceNumber;

    struct Request
    {
        std::string dstIp;
        std::promise<util::Result<ICMPResponse, IcmpError>> promise;
        TimePoint time;
    };
    std::map<std::string, Request> m_pendingRequests;

  public:
    ICMPScanner(v2::RawSocket::Ptr socket);

    util::Result<ICMPResponse, IcmpError> ping(std::string_view host) override;
    std::future<util::Result<ICMPResponse, IcmpError>> pingAsync(std::string_view host) override;

    bool hasPendingRequests() const override;

  private:
    void onPacketReceived(const v2::ReceivedPacket& packet) override;
    void handleTimeouts() override;
};
} // namespace net
