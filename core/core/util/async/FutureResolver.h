#pragma once

#include "Task.h"

#include <chrono>
#include <exception>
#include <future>
#include <ratio>

namespace util::v2
{
template <typename T>
class FutureResolver : public Task
{
    std::future<T> m_fut;
    std::chrono::milliseconds m_retryDelay;

  public:
    FutureResolver(std::chrono::milliseconds retryDelay = std::chrono::seconds(1))
        : m_retryDelay(std::move(retryDelay))
    {
    }

    virtual void onCompletion(const T& item) = 0;

    // optional
    virtual void onException(const std::exception_ptr&){};

    void setFuture(std::future<T>&& fut)
    {
        m_fut = std::move(fut);
    }

    void setRetryDelay(std::chrono::milliseconds delay)
    {
        m_retryDelay = std::move(delay);
    }

    const std::chrono::milliseconds& retryDelay()
    {
        return m_retryDelay;
    }

    Continuation run() override
    {
        if (m_fut.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
        {
            try
            {
                onCompletion(m_fut.get());
            }
            catch (...)
            {
                onException(std::current_exception());
            }

            return Task::End{};
        }

        return Task::RunAgainDelayed{
            retryDelay()
        };
    }
};
} // namespace util::v2