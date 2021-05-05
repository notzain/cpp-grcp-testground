#pragma once

#include "core/traits/Printable.h"

#include <boost/asio/generic/datagram_protocol.hpp>
#include <boost/asio/generic/raw_protocol.hpp>
#include <boost/asio/ip/icmp.hpp>
#include <string>

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

struct RawProtocol
{
    using Socket = boost::asio::generic::datagram_protocol::socket;

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
} // namespace net::v2