#pragma once

#include "AsyncSocket.h"
#include "ISocket.h"

#include <boost/asio.hpp>
#include <memory>
#include <vector>

#include "core/util/Result.h"

namespace net
{
class ICMPSocket
    : public ISocket
    , public ToAsync
{
    using SocketType = boost::asio::ip::icmp::socket;
    std::shared_ptr<SocketType> m_socket;

  public:
    explicit ICMPSocket(boost::asio::io_service& ioService);
    virtual ~ICMPSocket();
    bool connect() override;
    bool isConnected() const override;
    bool disconnect() override;
    void permitBroadcast(bool permit) override;

    nonstd::expected<std::size_t, std::string> send(void* data, std::size_t len) override;
    nonstd::expected<std::size_t, std::string> sendTo(std::string_view host, std::size_t port, void* data, std::size_t len) override;

    nonstd::expected<std::vector<std::uint8_t>, std::string> receive(std::string_view host, std::size_t port);

    std::shared_ptr<IAsyncSocket> toAsync() override;
};
} // namespace net
