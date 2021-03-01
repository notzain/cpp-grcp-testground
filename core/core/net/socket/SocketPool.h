#pragma once

#include "ISocketPool.h"

#include <boost/asio.hpp>
#include <thread>

namespace core::net
{

class SocketPool
    : public ISocketPool
{
    boost::asio::io_service m_ioService;
    boost::asio::io_service::work m_serviceKeepAlive;
    std::thread m_serviceThread;

  public:
    static SocketPool& defaultPool();

    SocketPool();
    virtual ~SocketPool();

    void shutdown();

    std::shared_ptr<core::net::ICMPSocket> createIcmpSocket() override;
    std::shared_ptr<core::net::RawSocket> createRawSocket(NetworkInterface& networkInterface) override;
};
} // namespace core::net