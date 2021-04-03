#include "RawSocket.h"
#include "core/net/NetworkInterface.h"

namespace net
{
RawSocket::RawSocket(NetworkInterface& networkInterface)
    : m_networkInterface(networkInterface)
{
}

bool RawSocket::connect()
{
    return false;
}
bool RawSocket::isConnected() const
{
    return false;
}
bool RawSocket::disconnect()
{
    return false;
}
void RawSocket::permitBroadcast(bool permit)
{
}
nonstd::expected<std::size_t, std::string> RawSocket::send(void* data, std::size_t len)
{
    return nonstd::make_unexpected("Not implemented");
}
nonstd::expected<std::size_t, std::string> RawSocket::sendTo(std::string_view host, std::size_t port, void* data, std::size_t len)
{
    try
    {
        m_networkInterface.device->open();
        m_networkInterface.device->sendPacket((uint8_t*)data, len);
        m_networkInterface.device->close();
        return len;
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

nonstd::expected<std::vector<std::uint8_t>, std::string> RawSocket::receive(std::string_view host, std::size_t port)
{
    return nonstd::make_unexpected("Not implemented");
}
} // namespace net