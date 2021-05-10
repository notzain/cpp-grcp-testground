#include "RawSocket.h"
#include "core/net/socket/v2/Protocols.h"

#include <boost/asio/ip/address_v4.hpp>
#include <linux/if_ether.h>
#include <sys/socket.h>

namespace net::v2
{
RawSocket::RawSocket(std::string_view interface, std::shared_ptr<RawProtocol::Socket> socket)
    : BoostSocketListener(socket)
    , m_socket(socket)
    , m_sockAddr(interface.data())
{
    m_socket->open(RawProtocol::Protocol(PF_PACKET, SOCK_RAW));
    m_socket->bind(boost::asio::generic::raw_protocol::endpoint(&m_sockAddr.sockAddr, sizeof(m_sockAddr.sockAddr)));
}

Result<std::size_t> RawSocket::send(nonstd::span<std::uint8_t> payload)
{
    try
    {
        return m_socket->send(boost::asio::buffer(payload.data(), payload.size()));
    }
    catch (std::exception& e)
    {
        CORE_WARN("Could not send data {}: {}",
                  m_sockAddr,
                  e.what());
        return Error(ErrorType::InvalidArgument);
    }
}

Result<std::size_t> RawSocket::sendTo(const RawProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> payload)
{
    try
    {
        RawProtocol::SockAddr addr(endpoint.interface);
        auto remote_endpoint = boost::asio::generic::raw_protocol::endpoint(&addr.sockAddr, sizeof(addr.sockAddr));

        return m_socket->send_to(boost::asio::buffer(payload.data(), payload.size()), remote_endpoint);
    }
    catch (std::exception& e)
    {
        CORE_WARN("Could not send data {}: {}",
                  endpoint,
                  e.what());
        return Error(ErrorType::InvalidArgument);
    }
}

} // namespace net::v2