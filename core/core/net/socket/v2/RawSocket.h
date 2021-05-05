#pragma once

#include "Protocols.h"
#include "ReceivingSocket.h"
#include "Socket.h"
#include <boost/asio/ip/address_v4.hpp>
#include <exception>

namespace net::v2
{
class RawSocket
    : public Socket<IcmpProtocol>
    , public BoostSocketListener<IcmpProtocol>
{
    std::shared_ptr<IcmpProtocol::Socket> m_socket;

  public:
    using Ptr = std::shared_ptr<RawSocket>;
    static RawSocket::Ptr create(std::shared_ptr<IcmpProtocol::Socket> socket)
    {
        auto self = std::shared_ptr<RawSocket>(new RawSocket(socket));
        self->startReceive();
        return self;
    }

    virtual ~RawSocket() = default;

    bool connect() override { return true; }
    bool isConnected() const override { return true; }
    bool disconnect() override { return true; }

    Result<std::size_t> send(nonstd::span<std::uint8_t> payload) override { return Error(ErrorType::Unimplemented); }
    Result<std::size_t> sendTo(const IcmpProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> payload) override
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

  private:
    RawSocket(std::shared_ptr<IcmpProtocol::Socket> socket)
        : BoostSocketListener(socket)
        , m_socket(socket)
    {
        m_socket->open(boost::asio::ip::icmp::v4());
    }
};
} // namespace net::v2