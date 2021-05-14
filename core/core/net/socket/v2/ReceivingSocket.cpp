#include "ReceivingSocket.h"

#include "core/util/logger/Logger.h"
#include <spdlog/fmt/bin_to_hex.h>

namespace net::v2
{
BoostSocketListenerBase::BoostSocketListenerBase()
    : m_packetBuffer(MaxBufferSize())
{
}

std::optional<ReceivedPacket> BoostSocketListenerBase::nextReceivedPacket()
{
    ReceivedPacket data;
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

    ReceivedPacket packet(length);

    m_asioBuffer.commit(length);
    std::istream is(&m_asioBuffer);

    is.read(reinterpret_cast<char*>(packet.bytes.data()), length);
    m_packetBuffer.push(packet);

    startReceive();
}

} // namespace net::v2