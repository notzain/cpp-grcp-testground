#include "IScanner.h"

namespace core::net
{
IAsyncScanner::IAsyncScanner(std::shared_ptr<IAsyncSocket> asyncSocket)
    : m_readStopToken(false)
    , m_asyncSocket(std::move(asyncSocket))
{
}

IAsyncScanner::~IAsyncScanner()
{
    stopReading();
}

void IAsyncScanner::startReading()
{
    m_readStopToken.store(false);
    m_readThread = std::thread([this] { read(); });
}

void IAsyncScanner::stopReading()
{
    m_readStopToken.store(true);
    if (m_readThread.joinable())
    {
        m_readThread.join();
    }
}

void IAsyncScanner::read()
{
    while (!m_readStopToken.load())
    {
        if (auto bytesRead = m_asyncSocket->nextReceivedPacket())
        {
            onPacketReceived(bytesRead->data(), bytesRead->size());
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

} // namespace core::net