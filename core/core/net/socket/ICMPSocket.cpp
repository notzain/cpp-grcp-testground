#include "ICMPSocket.h"

#include "core/util/logger/Logger.h"

namespace core::net
{
ICMPSocket::ICMPSocket(boost::asio::io_service& ioService)
    : m_socket(ioService)
{
    m_socket.open(boost::asio::ip::icmp::v4());
}

ICMPSocket::~ICMPSocket()
{
    try
    {
        m_socket.close();
    }
    catch (const std::exception& e)
    {
        CORE_WARN(e.what());
    }
}

bool ICMPSocket::connect()
{
    return false;
}

bool ICMPSocket::isConnected() const
{
    return false;
}

bool ICMPSocket::disconnect()
{
    return false;
}

void ICMPSocket::permitBroadcast(bool permit)
{
}

nonstd::expected<std::size_t, std::string> ICMPSocket::send(void* data, std::size_t len)
{
    return nonstd::make_unexpected("Not implemented");
}

nonstd::expected<std::size_t, std::string> ICMPSocket::sendTo(std::string_view host, std::size_t port, void* data, std::size_t len)
{
    try
    {
        auto remote_endpoint = boost::asio::ip::icmp::endpoint(boost::asio::ip::make_address_v4(host), port);

        return m_socket.send_to(boost::asio::buffer(data, len), remote_endpoint);
    }
    catch (std::exception& e)
    {
        return nonstd::make_unexpected(
            fmt::format("Could not send data to '{}' on port {}: {}",
                        host,
                        port,
                        e.what()));
    }
}

nonstd::expected<std::vector<std::uint8_t>, std::string> ICMPSocket::receive(std::string_view host, std::size_t port)
{
    auto remote_endpoint = boost::asio::ip::icmp::endpoint(boost::asio::ip::make_address_v4(host), port);

    try
    {
        typedef boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO> rcv_timeout_option;
        m_socket.set_option(rcv_timeout_option{ 5000 });

        std::array<std::uint8_t, 8196> buffer{};
        const auto bytesRead = m_socket.receive_from(boost::asio::buffer(buffer.data(), buffer.size()), remote_endpoint);

        return std::vector<std::uint8_t>(buffer.begin(), buffer.begin() + bytesRead);
    }
    catch (const std::exception& e)
    {
        return nonstd::make_unexpected(e.what());
    }
}

std::shared_ptr<IAsyncSocket> ICMPSocket::toAsync()
{
    return std::make_shared<AsyncSocket<SocketType>>(m_socket);
}
} // namespace core::net
