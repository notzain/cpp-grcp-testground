#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <mutex>
#include <thread>

#include "core/util/logger/Logger.h"

namespace util
{

class ThreadContext
{
    using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    boost::asio::io_context m_ioContext;
    std::thread m_thread;

    std::unique_ptr<work_guard_type> m_keepAlive;
    std::atomic_bool m_shutdown;

  public:
    using Ptr = std::shared_ptr<ThreadContext>;

    enum StopToken
    {
        CancelRemainingWork,
        FinishRemainingWork
    };

  public:
    ~ThreadContext()
    {
        stop();
        CORE_INFO("Stopping ThreadContext");
    }

    static ThreadContext::Ptr create()
    {
        return std::shared_ptr<ThreadContext>(new ThreadContext);
    }

    void start()
    {
        m_thread = std::thread([this] {
            m_shutdown.store(false);
            m_keepAlive = std::make_unique<work_guard_type>(m_ioContext.get_executor());
            m_ioContext.reset();
            m_ioContext.run();
        });
    }

    // Blocks till atleast current task is finished.
    void stop(StopToken token = CancelRemainingWork)
    {
        switch (token)
        {
        case CancelRemainingWork: {
            m_shutdown.store(true);
            m_ioContext.stop();
            break;
        }
        case FinishRemainingWork: {
            m_keepAlive.reset();
            break;
        }
        }

        if (m_thread.joinable())
            m_thread.join();
    }

    const boost::asio::io_context& ioContext() const { return m_ioContext; }
    boost::asio::io_context& ioContext() { return m_ioContext; }

    const std::thread& thread() const { return m_thread; }
    std::thread& thread() { return m_thread; }

    template <typename Func>
    void post(Func&& f)
    {
        boost::asio::post(m_ioContext, [this, func = std::move(f)] {
            // On Windows, calling ioContext.stop() keeps running handlers till the scheduler (randomly?)
            // decides to check the stop flag. On Unix, this is not an issue.
            // http://boost.2283326.n4.nabble.com/Asio-Does-stop-cancel-handlers-post-ed-to-io-service-td4648918.html
            if (m_shutdown.load())
                return;
            func();
        });
    }

  private:
    ThreadContext()
        : m_keepAlive(nullptr)
        , m_shutdown(false)
    {
    }
};

class ThreadContextRegistry
{
    inline static std::mutex m_registryMutex;
    inline static std::map<std::string, ThreadContext::Ptr> m_registry;

  public:
    static ThreadContext::Ptr registerThreadContext(const std::string& name)
    {
        std::lock_guard g(m_registryMutex);
        if (m_registry.count(name))
        {
            // Error or something
            return m_registry.at(name);
        }
        return createAndStart(name);
    }

    static ThreadContext::Ptr getDefault()
    {
        std::lock_guard g(m_registryMutex);
        return m_registry.at("default");
    }

    static ThreadContext::Ptr getOrDefault(const std::string& name, const std::string& fallback = "default")
    {
        std::lock_guard g(m_registryMutex);
        if (m_registry.count(name))
        {
            return m_registry.at(name);
        }

        if (m_registry.count(fallback))
        {
            return m_registry.at(fallback);
        }

        // Error here
        return nullptr;
    }

  private:
    ThreadContextRegistry()
    {
        registerThreadContext("default");
    };

    static ThreadContext::Ptr createAndStart(const std::string& name)
    {
        auto threadContext = m_registry.insert({ name, ThreadContext::create() }).first->second;
        threadContext->start();
        return threadContext;
    }
};
} // namespace util