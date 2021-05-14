#include "ReceivingSocket.h"

#include "core/util/logger/Logger.h"

namespace net::v2
{
BoostSocketListenerBase::BoostSocketListenerBase()
    : m_packetBuffer(1024)
{
}

std::optional<PacketByteContainer> BoostSocketListenerBase::nextReceivedPacket()
{
    PacketByteContainer data;
    if (m_packetBuffer.pop(data))
        return data;
    return std::nullopt;
}

void BoostSocketListenerBase::handleReceive(const boost::system::error_code& ec, std::size_t length)
{
    if (ec)
    {
        CORE_WARN(ec.message());
        startReceive();
        return;
    }

    CORE_INFO("Received {} bytes", length);
    m_asioBuffer.commit(length);
    std::istream is(&m_asioBuffer);

    PacketByteContainer data(length);
    is.read(reinterpret_cast<char*>(data.data()), length);
    m_packetBuffer.push(data);

    startReceive();
}

} // namespace net::v2