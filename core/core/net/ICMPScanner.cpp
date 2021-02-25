#include "ICMPScanner.h"

#include "core/util/logger/Logger.h"
#include <spdlog/fmt/bin_to_hex.h>

#include <boost/asio.hpp>
#include <thread>
#include <utility>

#include <ArpLayer.h>
#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <MacAddress.h>
#include <Packet.h>
#include <SystemUtils.h>
#include <pcap/pcap.h>

namespace core::net
{
ICMPScanner::ICMPScanner(NetworkInterface& networkInterface, std::shared_ptr<ICMPSocket> socket)
    : m_networkInterface(networkInterface)
    , m_socket(std::move(socket))
{
}

core::util::Result<int> ICMPScanner::ping(std::string_view host)
{
    pcpp::IcmpLayer icmpLayer;
    icmpLayer.setEchoRequestData(std::hash<std::thread::id>()(std::this_thread::get_id()),
                                 *m_sequenceNumber++,
                                 0,
                                 (const std::uint8_t*)"WHAT",
                                 5);
    icmpLayer.computeCalculateFields();

    pcpp::ArpLayer arpLayer(pcpp::ArpOpcode::ARP_REQUEST,
                            m_networkInterface.device->getMacAddress(),
                            pcpp::MacAddress::Zero,
                            m_networkInterface.device->getIPv4Address(),
                            pcpp::IPv4Address(host.data()));
    arpLayer.computeCalculateFields();
    if (auto bytesSent = m_socket->sendTo(host, 7, (std::uint8_t*)arpLayer.getData(), arpLayer.getDataLen()))
    {
        CORE_INFO("Sent {} bytes to {}", *bytesSent, host);
        if (auto bytesRead = m_socket->receive(host, 7))
        {
            const auto* ipHeader = reinterpret_cast<pcpp::iphdr*>(bytesRead->data());
            pcpp::icmp_echo_reply icmpReply{};
            icmpReply.header = reinterpret_cast<pcpp::icmp_echo_hdr*>(bytesRead->data() + sizeof(pcpp::iphdr));
            icmpReply.data = bytesRead->data() + sizeof(pcpp::iphdr) + sizeof(pcpp::icmp_echo_hdr);
            icmpReply.dataLength = &bytesRead->back() - icmpReply.data;
            CORE_INFO(spdlog::to_hex(*bytesRead, 16));
        }
        else
        {
            CORE_DEBUG_WARN(bytesRead.error());
        }
    }
    else
    {
        CORE_WARN(bytesSent.error());
    }
    return 1;
}

} // namespace core::net
