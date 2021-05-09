#pragma once

#include "ISocketPool.h"

#include <boost/asio.hpp>
#include <thread>

namespace net
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

    std::shared_ptr<net::v2::IcmpSocket> createIcmpSocketv2() override;
    std::shared_ptr<net::v2::RawSocket> createRawSocket(std::string_view interface) override;
};
} // namespace net
