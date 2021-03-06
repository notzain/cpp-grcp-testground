#include "IcmpScanner.h"

#include "core/net/MacAddress.h"
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
IcmpScanner::IcmpScanner(NetworkInterface networkInterface, v2::RawSocket::Ptr socket)
    : IAsyncScanner(socket)
    , m_networkInterface(std::move(networkInterface))
    , m_socket(socket)
{
    m_socket->attachFilter(IcmpFilter());
}

util::Result<IcmpResponse, IcmpError> IcmpScanner::ping(const IPv4Address& host)
{
    return ping(host, MacAddress::broadcast());
}

std::future<util::Result<IcmpResponse, IcmpError>> net::IcmpScanner::pingAsync(const IPv4Address& host)
{
    return pingAsync(host, MacAddress::broadcast());
}

util::Result<IcmpResponse, IcmpError> IcmpScanner::ping(const IPv4Address& host, const MacAddress& mac)
{
    auto fut = pingAsync(host, mac);
    if (fut.wait_for(std::chrono::seconds(5)) == std::future_status::timeout)
    {
        const auto& request = m_pendingRequests[host.toString()];
        m_pendingRequests.erase(host.toString());
        return Error(IcmpError{
            request.dstIp,
            ErrorType::TimedOut });
    }
    return fut.get();
}

std::future<util::Result<IcmpResponse, IcmpError>> IcmpScanner::pingAsync(const IPv4Address& host, const MacAddress& mac)
{
    PacketBuilder packetBuilder;
    packetBuilder
        .addLayer(EthLayerBuilder::create()
                      .withSrcMac(m_networkInterface.macAddress().toString())
                      .withDstMac(mac.toString()))
        .addLayer(IPv4LayerBuilder::create()
                      .withSrcIp(m_networkInterface.ipAddress().toString())
                      .withDstIp(host.toString())
                      .withId(htons(2000))
                      .withTimeToLive(64))
        .addLayer(IcmpLayerBuilder::create()
                      .withId(std::hash<std::thread::id>()(std::this_thread::get_id()))
                      .withSequence(0)
                      .withTimestamp()
                      .withPayload("WHAT"));

    const auto sentTime = SystemClock::now();
    if (auto bytesSent = m_socket->send(packetBuilder.getData()))
    {
        CORE_INFO("Sent {} bytes to {}", *bytesSent, host);
    }
    else
    {
        CORE_WARN("{}", bytesSent.error());
    }

    m_pendingRequests[host.toString()] = Request{ host, {}, sentTime };
    return m_pendingRequests[host.toString()].promise.get_future();
}

void IcmpScanner::onPacketReceived(const v2::ReceivedPacket& packet)
{
    PacketParser parser({ (uint8_t*)packet.bytes.data(), (size_t)packet.bytes.size() });
    auto* ethLayer = parser.getPacket().getLayerOfType<pcpp::EthLayer>();
    if (!ethLayer)
        return;

    auto ipLayer = parser.getPacket().getLayerOfType<pcpp::IPv4Layer>();
    if (!ipLayer)
        return;

    IcmpResponse response;
    response.srcIp = ipLayer->getSrcIpAddress();
    response.srcMac = ethLayer->getSourceMac();

    response.dstIp = ipLayer->getDstIpAddress();
    response.dstMac = ethLayer->getDestMac();

    response.ttl = ipLayer->getIPv4Header()->timeToLive;

    if (m_pendingRequests.count(response.srcIp.toString()))
    {
        auto& request = m_pendingRequests[response.srcIp.toString()];

        response.responseTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            packet.arrival - request.time);

        request.promise.set_value(std::move(response));

        m_pendingRequests.erase(response.srcIp.toString());
    }
}

void IcmpScanner::handleTimeouts()
{
    const auto now = std::chrono::system_clock::now();
    static auto defaultTimeout = std::chrono::seconds(5);
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); /* empty */)
    {
        auto& [_, req] = *it;
        if (now - req.time >= defaultTimeout)
        {
            req.promise.set_value(Error<IcmpError>({ req.dstIp,
                                                     ErrorType::TimedOut }));

            it = m_pendingRequests.erase(it);
        }
        else
        {
            it++;
        }
    }
}

bool IcmpScanner::hasPendingRequests() const
{
    return !m_pendingRequests.empty();
}
} // namespace net
