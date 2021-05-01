#pragma once

#include "AsyncSocket.h"
#include "Protocols.h"
#include "Socket.h"

namespace net::v2
{
class IcmpSocket
    : public Socket<IcmpProtocol>
    , public BoostAsyncSocket<IcmpProtocol>
{
    using Ptr = std::shared_ptr<IcmpSocket>;

  public:
    static IcmpSocket::Ptr create(boost::asio::io_service& ioService)
    {
        auto self = std::shared_ptr<IcmpSocket>(new IcmpSocket(ioService));
        self->startReceive();
        return self;
    }

    virtual ~IcmpSocket() = default;

    bool connect() override { return true; }
    bool isConnected() const override { return true; }
    bool disconnect() override { return true; }

    Result<std::size_t> send(nonstd::span<std::uint8_t> data) override { return Error(ErrorType::Unimplemented); }
    Result<std::size_t> sendTo(const IcmpProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> data) override { return Error(ErrorType::Unimplemented); }

    std::future<std::size_t> sendToAsync(const IcmpProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> payload) override
    {
        auto remote_endpoint = boost::asio::ip::icmp::endpoint(boost::asio::ip::make_address_v4(endpoint.ipAddress), endpoint.port);
        return m_socket->async_send_to(boost::asio::buffer(payload.data(), payload.size()), remote_endpoint, boost::asio::use_future);
    }

  private:
    IcmpSocket(boost::asio::io_service& ioService)
        : BoostAsyncSocket(std::make_shared<IcmpProtocol::Socket>(ioService))
    {
    }
};
} // namespace net::v2