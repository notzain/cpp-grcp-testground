#pragma once

#include "ISocket.h"
#include <boost/asio.hpp>

namespace net
{
class NetworkInterface;

class RawSocket : public ISocket
{
    NetworkInterface& m_networkInterface;

  public:
    explicit RawSocket(NetworkInterface& networkInterface);
    virtual ~RawSocket() = default;

    bool connect() override;
    bool isConnected() const override;
    bool disconnect() override;
    void permitBroadcast(bool permit) override;
    nonstd::expected<std::size_t, std::string> send(void* data, std::size_t len) override;
    nonstd::expected<std::size_t, std::string> sendTo(std::string_view host, std::size_t port, void* data, std::size_t len) override;

    nonstd::expected<std::vector<std::uint8_t>, std::string> receive(std::string_view host, std::size_t port);
};
} // namespace net
