#pragma once

#include <boost/asio/io_context.hpp>
#include <functional>
#include <memory>
#include <thread>
#include <type_traits>
#include <variant>

#include "core/util/Thread.h"

namespace util
{
struct Task
{
    struct RunAgain
    {
    };
    struct RunAgainDelayed
    {
        std::chrono::milliseconds delay;
    };
    struct End
    {
    };

    using Continuation = std::variant<RunAgain, RunAgainDelayed, End>;
    virtual Continuation run() = 0;
};

class OneShotTask : public Task
{
    std::function<void()> m_func;

  public:
    OneShotTask(std::function<void()>&& f)
        : m_func(std::move(f))
    {
    }

    Continuation run() override
    {
        m_func();
        return Task::End{};
    }
};

class TaskRunner
{
    boost::asio::io_context m_ioContext;
    boost::asio::io_context::work m_keepAlive;
    Thread m_serviceThread;
    std::shared_ptr<std::atomic_bool> m_isAlive = std::make_shared<std::atomic_bool>(true);

  public:
    TaskRunner();

    static TaskRunner& defaultRunner()
    {
        static auto instance = TaskRunner();
        return instance;
    }

    ~TaskRunner();

    void post(std::shared_ptr<Task> task);

    template <typename T, typename... Args>
    std::shared_ptr<T> post(Args&&... args)
    {
        static_assert(std::is_base_of_v<Task, T>, "T must derive from Task.");

        auto task = std::make_shared<T>(std::forward<Args>(args)...);
        post(task);
        return task;
    }

    void postDelayed(std::shared_ptr<Task> task, const std::chrono::milliseconds& delay);

    template <typename T, typename... Args>
    std::shared_ptr<T> postDelayed(Args&&... args, const std::chrono::milliseconds& delay)
    {
        static_assert(std::is_base_of<Task, T>(), "T must derive from Task.");

        auto task = std::make_shared<T>(std::forward<Args>(args)...);
        postDelayed(task, delay);
        return task;
    }

    void stop();

  private:
    void resolve(std::shared_ptr<Task> task);
};
} // namespace util

using util::OneShotTask;
using util::Task;
using util::TaskRunner;