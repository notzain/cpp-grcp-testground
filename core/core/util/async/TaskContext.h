#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <mutex>
#include <thread>

#include "core/util/Thread.h"
#include "core/util/logger/Logger.h"

namespace util
{

class TaskContext
{
    using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    boost::asio::io_context m_ioContext;
    std::unique_ptr<work_guard_type> m_keepAlive;

    Thread m_thread;

  public:
    using Ptr = std::shared_ptr<TaskContext>;

  public:
    ~TaskContext()
    {
        stop();
    }

    static TaskContext::Ptr create()
    {
        return TaskContext::Ptr(new TaskContext);
    }

    // Blocks till atleast current task is finished.
    void stop()
    {
        m_thread.requestStop();
    }

    const boost::asio::io_context& ioContext() const { return m_ioContext; }
    boost::asio::io_context& ioContext() { return m_ioContext; }

    template <typename Func>
    void post(Func&& f)
    {
        boost::asio::post(m_ioContext, [this, func = std::move(f)] {
            // On Windows, calling ioContext.stop() keeps running handlers till the scheduler (randomly?)
            // decides to check the stop flag. On Unix, this is not an issue.
            // http://boost.2283326.n4.nabble.com/Asio-Does-stop-cancel-handlers-post-ed-to-io-service-td4648918.html
            func();
        });
    }

  private:
    TaskContext()
        : m_keepAlive(nullptr)
        , m_thread(Thread::spawn("TaskContext", [this] {
            m_keepAlive = std::make_unique<work_guard_type>(m_ioContext.get_executor());
            m_ioContext.run();
        }))
    {
    }
};

class TaskContextRegistry
{
    inline static std::mutex m_registryMutex;
    inline static std::map<std::string, TaskContext::Ptr> m_registry;

  public:
    static TaskContext::Ptr registerTaskContext(const std::string& name)
    {
        std::lock_guard g(m_registryMutex);
        if (m_registry.count(name))
        {
            // Error or something
            return m_registry.at(name);
        }
        return createAndStart(name);
    }

    static TaskContext::Ptr getDefault()
    {
        std::lock_guard g(m_registryMutex);
        return m_registry.at("default");
    }

    static TaskContext::Ptr getOrDefault(const std::string& name, const std::string& fallback = "default")
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
    TaskContextRegistry()
    {
        registerTaskContext("default");
    };

    static TaskContext::Ptr createAndStart(const std::string& name)
    {
        auto taskContext = m_registry.insert({ name, TaskContext::create() }).first->second;
        return taskContext;
    }
};
} // namespace util