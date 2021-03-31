#pragma once

#include <atomic>
#include <cstdint>
#include <future>
#include <string_view>
#include <thread>
#include <vector>

#include "core/net/socket/AsyncSocket.h"
#include "core/traits/async/Resolver.h"
#include "core/util/Result.h"

namespace core::net
{
template <typename ResultType, typename ErrorType>
class IScanner
{
  public:
    virtual ~IScanner() = default;
    virtual util::Result<ResultType, ErrorType> ping(std::string_view host) = 0;
};

template <typename ResultType, typename ErrorType>
class IAsyncScanner
    : public IScanner<ResultType, ErrorType>
    , public traits::ExternalResolver
{
    std::shared_ptr<IAsyncSocket> m_asyncSocket;

  public:
    IAsyncScanner(std::shared_ptr<IAsyncSocket> asyncSocket)
        : traits::ExternalResolver(std::chrono::milliseconds(100))
        , m_asyncSocket(std::move(asyncSocket))
    {
    }

    virtual ~IAsyncScanner() = default;

    virtual std::future<util::Result<ResultType, ErrorType>> pingAsync(std::string_view host) = 0;

  private:
    traits::ExternalResolver::Action attemptRead() override
    {
        if (auto bytesRead = m_asyncSocket->nextReceivedPacket())
        {
            onPacketReceived(bytesRead->data(), bytesRead->size());
            return traits::ExternalResolver::Action::Continue;
        }
        else
        {
            handleTimeouts();
            return traits::ExternalResolver::Action::Timeout;
        }
    };

    virtual void onPacketReceived(std::uint8_t* bytes, std::size_t len) = 0;
    virtual void handleTimeouts() = 0;
};
} // namespace core::net
