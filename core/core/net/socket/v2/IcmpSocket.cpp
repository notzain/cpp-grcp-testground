#include "IcmpSocket.h"

#include <boost/asio/ip/address_v4.hpp>
#include <exception>

namespace net::v2
{
IcmpSocket::IcmpSocket(std::shared_ptr<IcmpProtocol::Socket> socket)
    : BoostSocketListener(socket)
    , m_socket(socket)
{
    m_socket->open(boost::asio::ip::icmp::v4());
}

IcmpSocket::Ptr IcmpSocket::create(std::shared_ptr<IcmpProtocol::Socket> socket)
{
    auto self = std::shared_ptr<IcmpSocket>(new IcmpSocket(socket));
    self->startReceive();
    return self;
}

util::Result<std::size_t> IcmpSocket::sendTo(const IcmpProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> payload)
{
    try
    {
        auto remote_endpoint = boost::asio::ip::icmp::endpoint(boost::asio::ip::make_address_v4(endpoint.ipAddress), endpoint.port);
        return m_socket->send_to(boost::asio::buffer(payload.data(), payload.size()), remote_endpoint);
    }
    catch (std::exception& e)
    {
        CORE_WARN("Could not send data {}: {}",
                  endpoint,
                  e.what());
        return Error(ErrorType::Unknown);
    }
}

} // namespace net::v2