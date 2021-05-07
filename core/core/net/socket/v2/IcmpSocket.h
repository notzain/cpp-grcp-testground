#pragma once

#include "Protocols.h"
#include "ReceivingSocket.h"
#include "Socket.h"

namespace net::v2
{
class IcmpSocket
    : public Socket<IcmpProtocol>
    , public BoostSocketListener<IcmpProtocol>
{
    std::shared_ptr<IcmpProtocol::Socket> m_socket;

  public:
    using Ptr = std::shared_ptr<IcmpSocket>;
    static IcmpSocket::Ptr create(std::shared_ptr<IcmpProtocol::Socket> socket);

    virtual ~IcmpSocket() = default;

    bool connect() override { return true; }
    bool isConnected() const override { return true; }
    bool disconnect() override { return true; }

    Result<std::size_t> send(nonstd::span<std::uint8_t> payload) override { return Error(ErrorType::Unimplemented); }
    Result<std::size_t> sendTo(const IcmpProtocol::Endpoint& endpoint, nonstd::span<std::uint8_t> payload) override;

  private:
    IcmpSocket(std::shared_ptr<IcmpProtocol::Socket> socket);
};
} // namespace net::v2