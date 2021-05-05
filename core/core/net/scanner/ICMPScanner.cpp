#include "ICMPScanner.h"

#include "IpAddress.h"
#include "MacAddress.h"
#include "core/net/scanner/IScanner.h"
#include "core/util/logger/Logger.h"

#include <chrono>
#include <cstdint>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <stdexcept>
#include <thread>
#include <utility>

#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <SystemUtils.h>

namespace net
{
ICMPScanner::ICMPScanner(v2::RawSocket::Ptr socket)
    : IAsyncScanner(socket)
    , m_socket(socket)
{
}

util::Result<ICMPResponse, IcmpError> ICMPScanner::ping(std::string_view host)
{
    auto fut = pingAsync(host);
    if (fut.wait_for(std::chrono::seconds(5)) == std::future_status::timeout)
    {
        const auto& request = m_pendingRequests[host.data()];
        m_pendingRequests.erase(host.data());
        return Error(IcmpError{
            pcpp::IPv4Address(host.data()),
            request.dstIp,
            ErrorType::TimedOut });
    }
    return fut.get();
}

std::future<util::Result<ICMPResponse, IcmpError>> net::ICMPScanner::pingAsync(std::string_view host)
{
    pcpp::IcmpLayer icmpLayer;
    icmpLayer.setEchoRequestData(std::hash<std::thread::id>()(std::this_thread::get_id()),
                                 *m_sequenceNumber++,
                                 0,
                                 (const std::uint8_t*)"WHAT",
                                 5);
    icmpLayer.computeCalculateFields();

    if (auto bytesSent = m_socket->sendTo({ host.data(), 7 }, { icmpLayer.getData(), icmpLayer.getDataLen() }))
    {
        CORE_INFO("Sent {} bytes to {}", *bytesSent, host);
    }
    else
    {
        CORE_WARN("{}", bytesSent.error());
    }

    m_pendingRequests[host.data()] = Request{ host.data(), {}, std::chrono::system_clock::now() };
    return m_pendingRequests[host.data()].promise.get_future();
}

void ICMPScanner::onPacketReceived(std::uint8_t* bytes, std::size_t len)
{
    const auto now = std::chrono::system_clock::now();

    const auto* ipHeader = reinterpret_cast<const pcpp::iphdr*>(bytes);
    pcpp::icmp_echo_reply icmpReply{};
    icmpReply.header = reinterpret_cast<pcpp::icmp_echo_hdr*>(bytes + sizeof(pcpp::iphdr));
    icmpReply.data = bytes + sizeof(pcpp::iphdr) + sizeof(pcpp::icmp_echo_hdr);
    icmpReply.dataLength = (bytes + len) - icmpReply.data;

    ICMPResponse response;
    response.srcIp = pcpp::IPv4Address(ipHeader->ipSrc);
    response.dstIp = pcpp::IPv4Address(ipHeader->ipDst);
    response.ttl = ipHeader->timeToLive;

    if (m_pendingRequests.count(response.srcIp.toString()))
    {
        auto& request = m_pendingRequests[response.srcIp.toString()];

        response.responseTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - request.time);

        request.promise.set_value(std::move(response));

        m_pendingRequests.erase(response.srcIp.toString());
    }
}

void ICMPScanner::handleTimeouts()
{
    const auto now = std::chrono::system_clock::now();
    static auto defaultTimeout = std::chrono::seconds(5);
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); /* empty */)
    {
        auto& [host, req] = *it;
        if (now - req.time >= defaultTimeout)
        {
            req.promise.set_value(Error(IcmpError{
                pcpp::IPv4Address(host),
                req.dstIp,
                ErrorType::TimedOut }));

            it = m_pendingRequests.erase(it);
        }
        else
        {
            it++;
        }
    }
}
} // namespace net
