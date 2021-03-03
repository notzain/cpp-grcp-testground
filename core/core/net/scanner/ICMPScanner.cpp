#include "ICMPScanner.h"

#include "IpAddress.h"
#include "core/net/scanner/IScanner.h"
#include "core/util/logger/Logger.h"
#include <spdlog/fmt/bin_to_hex.h>

#include <boost/asio.hpp>
#include <thread>
#include <utility>

#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <SystemUtils.h>

namespace core::net
{
ICMPScanner::ICMPScanner(std::shared_ptr<ICMPSocket> socket)
    : IAsyncScanner(socket->toAsync())
    , m_socket(std::move(socket))
{
}

util::Result<int> ICMPScanner::ping(std::string_view host)
{
    pcpp::IcmpLayer icmpLayer;
    icmpLayer.setEchoRequestData(std::hash<std::thread::id>()(std::this_thread::get_id()),
                                 *m_sequenceNumber++,
                                 0,
                                 (const std::uint8_t*)"WHAT",
                                 5);
    icmpLayer.computeCalculateFields();

    if (auto bytesSent = m_socket->sendTo(host, 7, icmpLayer.getData(), icmpLayer.getDataLen()))
    {
        CORE_INFO("Sent {} bytes to {}", *bytesSent, host);
    }
    else
    {
        CORE_DEBUG_WARN(bytesSent.error());
    }
    return 1;
}

void ICMPScanner::onPacketReceived(std::uint8_t* bytes, std::size_t len)
{
    const auto* ipHeader = reinterpret_cast<const pcpp::iphdr*>(bytes);
    pcpp::icmp_echo_reply icmpReply{};
    icmpReply.header = reinterpret_cast<pcpp::icmp_echo_hdr*>(bytes + sizeof(pcpp::iphdr));
    icmpReply.data = bytes + sizeof(pcpp::iphdr) + sizeof(pcpp::icmp_echo_hdr);
    icmpReply.dataLength = (bytes + len) - icmpReply.data;

    CORE_INFO("Got reply from '{}': {}",
              pcpp::IPv4Address(ipHeader->ipSrc).toString(),
              spdlog::to_hex(bytes, bytes + len, 16));
}
} // namespace core::net
