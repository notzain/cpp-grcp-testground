#include "Resolver.h"

namespace core::traits
{
detail::Resolver::~Resolver()
{
    stop();
}

void detail::Resolver::start()
{
    m_readStopToken.store(false);
    m_readThread = std::thread([this] { readLoop(); });
}

void detail::Resolver::stop()
{
    m_readStopToken.store(true);
    if (m_readThread.joinable())
    {
        m_readThread.join();
    }
}

void detail::Resolver::readLoop()
{
    while (!m_readStopToken.load())
    {
        read();
    }
}
} // namespace core::traits