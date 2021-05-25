#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string_view>

#include "IScanner.h"
#include "IpAddress.h"
#include "MacAddress.h"
#include "core/net/IPv4Address.h"
#include "core/net/MacAddress.h"
#include "core/net/scanner/IScanner.h"
#include "core/net/socket/v2/IcmpSocket.h"
#include "core/net/socket/v2/Protocols.h"
#include "core/net/socket/v2/RawSocket.h"
#include "core/util/Result.h"
#include "core/util/type/WrapAround.h"

namespace net
{

struct IcmpResponse
{
    IPv4Address srcIp;
    IPv4Address dstIp;

    MacAddress srcMac;
    MacAddress dstMac;

    std::uint8_t ttl;
    std::chrono::milliseconds responseTime;
};

struct IcmpError
{
    IPv4Address dstIp;
    ErrorType error;
};

class IcmpScanner : public IAsyncScanner<IcmpResponse, IcmpError>
{
    v2::RawSocket::Ptr m_socket;
    // v2::RawSocket::Ptr m_rawSocket;
    util::WrapAround<std::uint8_t> m_sequenceNumber;

    struct Request
    {
        IPv4Address dstIp;
        std::promise<util::Result<IcmpResponse, IcmpError>> promise;
        TimePoint time;
    };
    std::map<std::string, Request> m_pendingRequests;

  public:
    IcmpScanner(v2::RawSocket::Ptr socket);

    util::Result<IcmpResponse, IcmpError> ping(const IPv4Address& host) override;
    std::future<util::Result<IcmpResponse, IcmpError>> pingAsync(const IPv4Address& host) override;

    bool hasPendingRequests() const override;

  private:
    void onPacketReceived(const v2::ReceivedPacket& packet) override;
    void handleTimeouts() override;
};
} // namespace net
