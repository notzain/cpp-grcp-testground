#pragma once

#include "core/util/logger/Logger.h"

#include <boost/asio.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <optional>
#include <vector>

namespace core::net
{

using PacketByteContainer = boost::container::static_vector<std::uint8_t, 128>;

struct IAsyncSocket
{
    virtual ~IAsyncSocket() = default;
    virtual std::future<std::size_t> sendToAsync(std::string_view host, std::size_t port, void* data, std::size_t len) = 0;
    virtual std::optional<PacketByteContainer> nextReceivedPacket() = 0;
};

struct ToAsync
{
    virtual ~ToAsync() = default;
    virtual std::shared_ptr<IAsyncSocket> toAsync() = 0;
};

template <typename Socket>
class AsyncSocket : public IAsyncSocket
{
    Socket& m_socket;
    boost::asio::streambuf m_asioBuffer;

    boost::lockfree::spsc_queue<boost::container::static_vector<std::uint8_t, 1024>> m_packetBuffer;

  public:
    explicit AsyncSocket(Socket& socket)
        : m_socket(socket)
        , m_packetBuffer(1024)
    {
        startReceive();
    }

    std::future<std::size_t> sendToAsync(std::string_view host, std::size_t port, void* data, std::size_t len) override
    {
        auto remote_endpoint = boost::asio::ip::icmp::endpoint(boost::asio::ip::make_address_v4(host), port);
        return m_socket.async_send_to(boost::asio::buffer(data, len), remote_endpoint, boost::asio::use_future);
    }

    std::optional<PacketByteContainer> nextReceivedPacket()
    {
        PacketByteContainer data;
        if (m_packetBuffer.pop(data))
        {
            return data;
        }
        return std::nullopt;
    }

  private:
    void startReceive()
    {
        // Discard any data already in the buffer.
        m_asioBuffer.consume(m_asioBuffer.size());

        // Wait for a reply. We prepare the buffer to receive up to 64KB.
        m_socket.async_receive(m_asioBuffer.prepare(65536),
                               [this](const boost::system::error_code& ec, std::size_t length) { handleReceive(ec, length); });
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
} // namespace core::net
