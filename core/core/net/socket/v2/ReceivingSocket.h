#pragma once

#include "core/util/logger/Logger.h"

#include <boost/asio.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <chrono>
#include <future>
#include <memory>
#include <nonstd/span.hpp>
#include <optional>

namespace net::v2
{
using PacketByteContainer = boost::container::static_vector<std::uint8_t, 128>;

class SocketListener
{
  public:
    virtual ~SocketListener() = default;
    virtual std::optional<PacketByteContainer> nextReceivedPacket() = 0;
};

template <typename Protocol>
class BoostSocketListener
    : public SocketListener
    , public std::enable_shared_from_this<BoostSocketListener<Protocol>>
{
    using Socket = typename Protocol::Socket;
    std::shared_ptr<Socket> m_socket;

    boost::asio::streambuf m_asioBuffer;
    boost::lockfree::spsc_queue<PacketByteContainer> m_packetBuffer;

  public:
    std::optional<PacketByteContainer> nextReceivedPacket() override
    {
        PacketByteContainer data;
        if (m_packetBuffer.pop(data))
        {
            return data;
        }
        return std::nullopt;
    }

  protected:
    BoostSocketListener(std::shared_ptr<Socket> socket)
        : m_socket(std::move(socket))
        , m_packetBuffer(1024)
    {
    }

    void startReceive()
    {
        // Discard any data already in the buffer.
        m_asioBuffer.consume(m_asioBuffer.size());

        // Wait for a reply. We prepare the buffer to receive up to 64KB.
        m_socket->async_receive(m_asioBuffer.prepare(65536),
                                [self = this->shared_from_this()](const boost::system::error_code& ec, std::size_t length) { self->handleReceive(ec, length); });
    }

    void handleReceive(const boost::system::error_code& ec, std::size_t length)
    {
        if (ec)
        {
            CORE_WARN(ec.message());
            startReceive();
            return;
        }

        m_asioBuffer.commit(length);
        std::istream is(&m_asioBuffer);

        PacketByteContainer data(length);
        is.read(reinterpret_cast<char*>(data.data()), length);

        m_packetBuffer.push(data);

        startReceive();
    }
};
} // namespace net::v2