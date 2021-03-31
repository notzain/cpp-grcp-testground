#pragma once

#include <boost/lockfree/queue.hpp>
#include <deque>
#include <future>
#include <mutex>

#include "core/util/logger/Logger.h"

namespace core::traits
{
namespace detail
{
class Resolver
{
    std::atomic_bool m_readStopToken;
    std::thread m_readThread;

  public:
    Resolver() = default;
    virtual ~Resolver();

    void start();
    void stop();

  private:
    void readLoop();
    virtual void read() = 0;
};
} // namespace detail

class ExternalResolver : public detail::Resolver
{
    std::chrono::milliseconds m_cycleTimeout;

  public:
    ExternalResolver(std::chrono::milliseconds cycleTimeout)
        : m_cycleTimeout(std::move(cycleTimeout))
    {
    }
    virtual ~ExternalResolver() = default;

    enum class Action
    {
        Continue,
        Timeout
    };

  private:
    virtual Action attemptRead() = 0;

    void read() override
    {
        if (attemptRead() == Action::Timeout)
        {
            std::this_thread::sleep_for(m_cycleTimeout);
        }
    }
};

template <typename T>
class QueueResolver : public detail::Resolver
{
    boost::lockfree::queue<T> m_queue;
    std::chrono::milliseconds m_cycleTimeout;

  public:
    QueueResolver(std::chrono::milliseconds cycleTimeout)
        : m_cycleTimeout(std::move(cycleTimeout))
    {
    }

    virtual ~QueueResolver() = default;

    virtual void onNextItem(const T& item) = 0;

    void enqueue(T&& item)
    {
        m_queue.push(item);
    }

    bool empty() const
    {
        return m_queue.empty();
    }

  private:
    void read() override
    {
        if (!m_queue.consume_one(&onNextItem))
        {
            std::this_thread::sleep_for(m_cycleTimeout);
        }
    }
};

template <typename Id, typename T>
class FutureResolver : public detail::Resolver
{
    std::mutex m_queueMutex;
    std::deque<std::pair<Id, std::future<T>>> m_queue;

    std::chrono::milliseconds m_cycleTimeout;

  public:
    FutureResolver(std::chrono::milliseconds cycleTimeout)
        : m_cycleTimeout(std::move(cycleTimeout))
    {
    }

    virtual ~FutureResolver()
    {
        stop();
    }

    virtual void onSuccess(const T& item) = 0;

    virtual void onFailure(const Id& id, std::string_view error) = 0;

    void enqueue(const Id& id, std::future<T>&& item)
    {
        std::lock_guard g(m_queueMutex);
        m_queue.emplace_back(id, std::move(item));
    }

    bool empty() const
    {
        std::lock_guard g(m_queueMutex);
        return m_queue.empty();
    }

    std::size_t size() const
    {
        std::lock_guard g(m_queueMutex);
        return m_queue.size();
    }

  private:
    void read() override
    {
        std::lock_guard g(m_queueMutex);
        auto readyFuture = std::find_if(m_queue.begin(), m_queue.end(), [](const auto& item) {
            const auto& [_, fut] = item;
            return fut.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
        });

        if (readyFuture != m_queue.end())
        {
            auto& [id, fut] = *readyFuture;
            try
            {
                onSuccess(fut.get());
            }
            catch (const std::exception& e)
            {
                onFailure(id, e.what());
            }
            m_queue.erase(readyFuture);
        }
        else
        {
            std::this_thread::sleep_for(m_cycleTimeout);
        }
    }
};
} // namespace core::traits