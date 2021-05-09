#pragma once

#include "Protocols.h"
#include "ReceivingSocket.h"
#include "Socket.h"

namespace net::v2
{
class RawSocket
    : public Socket<RawProtocol>
    , public BoostSocketListener<RawProtocol>
{
    std::shared_ptr<RawProtocol::Socket> m_socket;
    RawProtocol::SockAddr m_sockAddr;

  public:
    using Ptr = std::shared_ptr<RawSocket>;
    static RawSocket::Ptr create(std::string_view interface, std::shared_ptr<RawProtocol::Socket> socket)
    {
        auto self = std::shared_ptr<RawSocket>(new RawSocket(interface, socket));
        self->startReceive();
        return self;
    }

    virtual ~RawSocket() = default;

    bool connect() override { return true; }
    bool isConnected() const override { return true; }
    bool disconnect() override { return true; }

    Result<std::size_t> send(nonstd::span<std::uint8_t> payload) override { return Error(ErrorType::Unimplemented); }
    Result<std::size_t> sendTo(const RawProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> payload) override;

  private:
    RawSocket(std::string_view interface, std::shared_ptr<RawProtocol::Socket> socket);
};
} // namespace net::v2