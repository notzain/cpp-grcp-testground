#pragma once

#include "core/traits/Printable.h"

#include <boost/asio/generic/datagram_protocol.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/asio/ip/icmp.hpp>
#include <linux/if_ether.h>
#include <net/if.h>
#include <string>
#include <sys/socket.h>

namespace net::v2
{
struct IcmpProtocol
{
    using Socket = boost::asio::ip::icmp::socket;

    struct Endpoint : public traits::Printable<Endpoint>
    {
        std::string ipAddress;
        int port;

        Endpoint(std::string ip, int port)
            : ipAddress(std::move(ip))
            , port(port)
        {
        }

        std::string format() const override
        {
            return fmt::format("['{}', '{}']", ipAddress, port);
        }
    };
};

#ifdef __unix__
#include <linux/if_packet.h>
struct RawProtocol
{
    using Socket = boost::asio::generic::raw_protocol::socket;
    using Protocol = boost::asio::generic::raw_protocol;

    struct Endpoint : public traits::Printable<Endpoint>
    {
        // std::string ipAddress;
        // int port;
        std::string interface;

        // Endpoint(std::string ip, int port)
        //     : ipAddress(std::move(ip))
        //     , port(port)
        // {
        // }
        Endpoint(std::string iface)
            : interface(std::move(iface))
        {
        }

        std::string format() const override
        {
            return fmt::format("['{}']", interface);
        }
    };
    struct SockAddr
    {
        sockaddr_ll sockAddr;

        SockAddr(std::string_view interface)
        {
            memset(&sockAddr, 0, sizeof(sockAddr));
            sockAddr.sll_family = PF_PACKET;
            sockAddr.sll_protocol = htons(ETH_P_ALL);
            sockAddr.sll_ifindex = if_nametoindex(interface.data());
            sockAddr.sll_hatype = 1;
        }
    };
};
#endif
} // namespace net::v2