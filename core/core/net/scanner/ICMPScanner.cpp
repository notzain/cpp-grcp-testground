#include "ICMPScanner.h"

#include "IpAddress.h"
#include "MacAddress.h"
#include "core/net/packet/PacketBuilder.h"
#include "core/net/packet/layer/EthLayerBuilder.h"
#include "core/net/packet/layer/IPv4LayerBuilder.h"
#include "core/net/packet/layer/IcmpLayerBuilder.h"
#include "core/net/scanner/IScanner.h"
#include "core/net/socket/SocketPool.h"
#include "core/net/socket/v2/IcmpSocket.h"
#include "core/util/logger/Logger.h"

#include <Packet.h>
#include <RawPacket.h>
#include <chrono>
#include <cstdint>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <spdlog/fmt/bin_to_hex.h>
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
    PacketBuilder packetBuilder;
    packetBuilder
        .addLayer(EthLayerBuilder::create()
                      .withSrcMac("08:00:27:21:f8:3c")
                      .withDstMac("ff:ff:ff:ff:ff:ff"))
        .addLayer(IPv4LayerBuilder::create()
                      .withSrcIp("192.168.178.165")
                      .withDstIp("192.168.178.1")
                      .withId(htons(2000))
                      .withTimeToLive(64))
        .addLayer(IcmpLayerBuilder::create()
                      .withId(std::hash<std::thread::id>()(std::this_thread::get_id()))
                      .withSequence(0)
                      .withTimestamp()
                      .withPayload("WHAT"));

    if (auto bytesSent = m_socket->sendTo({ "enp0s3" }, packetBuilder.getData()))
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
            req.promise.set_value(Error<IcmpError>({ pcpp::IPv4Address(host),
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
bool ICMPScanner::hasPendingRequests() const
{
    return !m_pendingRequests.empty();
}
} // namespace net
