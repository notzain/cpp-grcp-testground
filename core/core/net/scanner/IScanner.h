#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <ratio>
#include <string_view>
#include <thread>
#include <vector>

#include "core/net/socket/AsyncSocket.h"
#include "core/util/Result.h"
#include "core/util/async/Resolver.h"
#include "core/util/async/Task.h"

namespace net
{
template <typename ResultType, typename ErrorType>
class IScanner
{
  public:
    virtual ~IScanner() = default;
    virtual util::Result<ResultType, ErrorType> ping(std::string_view host) = 0;

    virtual void start() = 0;
    virtual void stop() = 0;
};

template <typename ResultType, typename ErrorType>
class IAsyncScanner
    : public IScanner<ResultType, ErrorType>
{
    std::shared_ptr<IAsyncSocket> m_asyncSocket;
    std::atomic_bool m_running = false;

    struct PingResolver : public util::Task
    {
        IAsyncScanner* scanner;

        PingResolver(IAsyncScanner* scanner)
            : scanner(scanner)
        {
        }

        Continuation run() override
        {
            if (!scanner->m_running.load())
            {
                return util::Task::End{};
            }
            else if (auto bytesRead = scanner->m_asyncSocket->nextReceivedPacket())
            {
                scanner->onPacketReceived(bytesRead->data(), bytesRead->size());
                return util::Task::RunAgain{};
            }
            else
            {
                scanner->handleTimeouts();
                return util::Task::RunAgainDelayed{
                    std::chrono::milliseconds(100)
                };
            }
        }
    };

    std::weak_ptr<PingResolver> m_currentTask;

  public:
    IAsyncScanner(std::shared_ptr<IAsyncSocket> asyncSocket)
        : m_asyncSocket(std::move(asyncSocket))
    {
    }

    virtual ~IAsyncScanner() = default;

    virtual std::future<util::Result<ResultType, ErrorType>> pingAsync(std::string_view host) = 0;

    void start() override
    {
        m_running.store(true);

        auto task = std::make_shared<PingResolver>(this);
        m_currentTask = task;
        util::TaskRunner::defaultRunner()
            .post(task);
    }

    // blocks till task is ended
    void stop() override
    {
        m_running.store(false);
        while (!m_currentTask.expired())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

  private:
    virtual void onPacketReceived(std::uint8_t* bytes, std::size_t len) = 0;
    virtual void handleTimeouts() = 0;
};
} // namespace net
