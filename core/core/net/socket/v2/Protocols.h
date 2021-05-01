#pragma once

#include <string>

#include <boost/asio/ip/icmp.hpp>

namespace net::v2
{
struct IcmpProtocol
{
    using Socket = boost::asio::ip::icmp::socket;

    struct Endpoint
    {
        std::string ipAddress;
        int port;
    };
};
} // namespace net::v2