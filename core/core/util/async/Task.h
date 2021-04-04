#pragma once

#include <boost/asio/io_context.hpp>
#include <memory>
#include <thread>
#include <variant>

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

class TaskRunner
{
    boost::asio::io_context m_ioContext;
    boost::asio::io_context::work m_keepAlive;
    std::thread m_serviceThread;
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

    void postDelayed(std::shared_ptr<Task> task, const std::chrono::milliseconds& delay);

    void stop();

  private:
    void resolve(std::shared_ptr<Task> task);
};
} // namespace util