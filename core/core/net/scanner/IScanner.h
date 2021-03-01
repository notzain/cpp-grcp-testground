#pragma once

#include <atomic>
#include <cstdint>
#include <string_view>
#include <thread>
#include <vector>

#include "core/net/socket/AsyncSocket.h"
#include "core/util/Result.h"

namespace core::net
{
class IScanner
{
  public:
    virtual ~IScanner() = default;
    virtual util::Result<int> ping(std::string_view host) = 0;
};

class IAsyncScanner : public IScanner
{
    std::atomic_bool m_readStopToken;
    std::thread m_readThread;

    std::shared_ptr<IAsyncSocket> m_asyncSocket;

  public:
    IAsyncScanner(std::shared_ptr<IAsyncSocket> asyncSocket);
    virtual ~IAsyncScanner();

    void startReading();
    void stopReading();

    virtual void onPacketReceived(std::uint8_t* bytes, std::size_t len) = 0;

  private:
    void read();
};
} // namespace core::net
