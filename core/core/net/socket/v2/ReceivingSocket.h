#pragma once

#include "core/util/Time.h"

#include <boost/asio.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <future>
#include <memory>
#include <nonstd/span.hpp>
#include <optional>

namespace net::v2
{
constexpr inline std::size_t MaxBufferSize() { return 1024; };
struct ReceivedPacket
{
    TimePoint arrival;
    boost::container::static_vector<std::uint8_t, MaxBufferSize()> bytes;

    ReceivedPacket() = default;
    ReceivedPacket(std::size_t length)
        : arrival(std::chrono::system_clock::now())
        , bytes(length)
    {
    }
};

class SocketListener
{
  public:
    virtual ~SocketListener() = default;
    virtual std::optional<ReceivedPacket> nextReceivedPacket() = 0;
};

class BoostSocketListenerBase : public SocketListener
{
  protected:
    boost::asio::streambuf m_asioBuffer;
    boost::lockfree::spsc_queue<ReceivedPacket> m_packetBuffer;

  public:
    BoostSocketListenerBase();
    std::optional<ReceivedPacket> nextReceivedPacket() override;

  protected:
    virtual void startReceive() = 0;
    virtual void handleReceive(const boost::system::error_code& ec, std::size_t length);
};

template <typename Protocol>
class BoostSocketListener
    : public BoostSocketListenerBase
    , public std::enable_shared_from_this<BoostSocketListener<Protocol>>
{
    using Socket = typename Protocol::Socket;
    std::shared_ptr<Socket> m_socket;

  protected:
    BoostSocketListener(std::shared_ptr<Socket> socket)
        : m_socket(std::move(socket))
    {
    }

    void startReceive() override
    {
        // Discard any data already in the buffer.
        m_asioBuffer.consume(m_asioBuffer.size());

        // Wait for a reply. We prepare the buffer to receive up to 64KB.
        m_socket->async_receive(m_asioBuffer.prepare(MaxBufferSize()),
                                [self = this->shared_from_this()](const boost::system::error_code& ec, std::size_t length) { self->handleReceive(ec, length); });
    }
};
} // namespace net::v2