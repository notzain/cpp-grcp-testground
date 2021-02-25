#pragma once

#include "ISocket.h"

#include <boost/asio.hpp>
#include <future>
#include <vector>

#include "core/util/Result.h"

namespace core::net
{
class ICMPSocket : public ISocket
{
    boost::asio::ip::icmp::socket m_socket;

  public:
    explicit ICMPSocket(boost::asio::io_service& ioService);
    bool connect() override;
    bool isConnected() const override;
    bool disconnect() override;
    void permitBroadcast(bool permit) override;

    nonstd::expected<std::size_t, std::string> send(void* data, std::size_t len) override;
    nonstd::expected<std::size_t, std::string> sendTo(std::string_view host, std::size_t port, void* data, std::size_t len) override;

    std::future<std::size_t> sendToAsync(std::string_view host, std::size_t port, void* data, std::size_t len);

    nonstd::expected<std::vector<std::uint8_t>, std::string> receive(std::string_view host, std::size_t port);
};
} // namespace core::net