#include "SocketPool.h"

#include "v2/IcmpSocket.h"
#include "v2/Protocols.h"
#include "v2/RawSocket.h"

namespace net
{
SocketPool::SocketPool()
    : m_ioService()
    , m_serviceKeepAlive(m_ioService)
    , m_serviceThread([this] {
        m_ioService.run();
    })
{
}

SocketPool::~SocketPool()
{
    shutdown();
}

void SocketPool::shutdown()
{
    if (m_serviceThread.joinable())
    {
        m_ioService.stop();
        m_serviceThread.join();
    }
}

SocketPool& SocketPool::defaultPool()
{
    static SocketPool socketPool;
    return socketPool;
}

std::shared_ptr<net::v2::IcmpSocket> SocketPool::createIcmpSocketv2()
{
    return v2::IcmpSocket::create(std::make_shared<v2::IcmpProtocol::Socket>(m_ioService));
}

std::shared_ptr<v2::RawSocket> SocketPool::createRawSocket(std::string_view interface)
{
    try
    {
        return v2::RawSocket::create(interface, std::make_shared<v2::RawProtocol::Socket>(m_ioService));
    }
    catch (std::exception& e)
    {
        CORE_WARN(e.what());
        return nullptr;
    }
}

} // namespace net