#include "ICMPScanner.h"

#include "core/net/packet/PacketBuilder.h"
#include "core/net/packet/PacketParser.h"
#include "core/net/packet/layer/EthLayerBuilder.h"
#include "core/net/packet/layer/IPv4LayerBuilder.h"
#include "core/net/packet/layer/IcmpLayerBuilder.h"
#include "core/net/scanner/IScanner.h"
#include "core/net/socket/SocketPool.h"
#include "core/net/socket/filter/IcmpFilter.h"
#include "core/net/socket/v2/IcmpSocket.h"
#include "core/util/logger/Logger.h"

#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <IpAddress.h>
#include <MacAddress.h>
#include <Packet.h>
#include <RawPacket.h>
#include <SystemUtils.h>
#include <chrono>
#include <cstdint>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <stdexcept>
#include <thread>
#include <utility>

namespace net
{
ICMPScanner::ICMPScanner(v2::RawSocket::Ptr socket)
    : IAsyncScanner(socket)
    , m_socket(socket)
{
    m_socket->attachFilter(IcmpFilter());
}

util::Result<ICMPResponse, IcmpError> ICMPScanner::ping(const IPv4Address& host)
{
    auto fut = pingAsync(host);
    if (fut.wait_for(std::chrono::seconds(5)) == std::future_status::timeout)
    {
        const auto& request = m_pendingRequests[host.asString()];
        m_pendingRequests.erase(host.asString());
        return Error(IcmpError{
            host,
            request.dstIp,
            ErrorType::TimedOut });
    }
    return fut.get();
}

std::future<util::Result<ICMPResponse, IcmpError>> net::ICMPScanner::pingAsync(const IPv4Address& host)
{
    PacketBuilder packetBuilder;
    packetBuilder
        .addLayer(EthLayerBuilder::create()
                      .withSrcMac("08:00:27:21:f8:3c")
                      .withDstMac("ff:ff:ff:ff:ff:ff"))
        .addLayer(IPv4LayerBuilder::create()
                      .withSrcIp("192.168.178.165")
                      .withDstIp(host.asString())
                      .withId(htons(2000))
                      .withTimeToLive(64))
        .addLayer(IcmpLayerBuilder::create()
                      .withId(std::hash<std::thread::id>()(std::this_thread::get_id()))
                      .withSequence(0)
                      .withTimestamp()
                      .withPayload("WHAT"));

    const auto sentTime = SystemClock::now();
    if (auto bytesSent = m_socket->sendTo({ "enp0s3" }, packetBuilder.getData()))
    {
        CORE_INFO("Sent {} bytes to {}", *bytesSent, host);
    }
    else
    {
        CORE_WARN("{}", bytesSent.error());
    }

    m_pendingRequests[host.asString()] = Request{ host, {}, sentTime };
    return m_pendingRequests[host.asString()].promise.get_future();
}

void ICMPScanner::onPacketReceived(const v2::ReceivedPacket& packet)
{
    PacketParser parser({ (uint8_t*)packet.bytes.data(), (size_t)packet.bytes.size() });
    auto* ethLayer = parser.getPacket().getLayerOfType<pcpp::EthLayer>();
    if (!ethLayer)
        return;

    auto ipLayer = parser.getPacket().getLayerOfType<pcpp::IPv4Layer>();
    if (!ipLayer)
        return;

    ICMPResponse response;
    response.srcIp = ipLayer->getSrcIpAddress();
    response.srcMac = ethLayer->getSourceMac();

    response.dstIp = ipLayer->getDstIpAddress();
    response.dstMac = ethLayer->getDestMac();

    response.ttl = ipLayer->getIPv4Header()->timeToLive;

    if (m_pendingRequests.count(response.srcIp.asString()))
    {
        auto& request = m_pendingRequests[response.srcIp.asString()];

        response.responseTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            packet.arrival - request.time);

        request.promise.set_value(std::move(response));

        m_pendingRequests.erase(response.srcIp.asString());
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
            req.promise.set_value(Error<IcmpError>({ IPv4Address::parse(host).value_or(IPv4Address::zero()),
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
