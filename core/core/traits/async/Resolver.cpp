#include "Resolver.h"
#include <chrono>
#include <ratio>
#include <thread>

namespace traits
{

detail::Resolver::~Resolver()
{
}

void detail::Resolver::start()
{
}

void detail::Resolver::stop()
{
}

void detail::Resolver::readLoop()
{
}

TaskRunner::TaskRunner()
    : m_keepAlive(m_ioContext)
    , m_serviceThread([this] {
        m_ioContext.run();
    })
{
}

TaskRunner::~TaskRunner()
{
    m_isAlive->store(false);
    stop();
}

void TaskRunner::post(std::shared_ptr<Task> task)
{
    if (!m_ioContext.stopped())
    {
        boost::asio::post(m_ioContext, [this, task] {
            resolve(task);
        });
    }
}

void TaskRunner::stop()
{
    try
    {
        if (m_serviceThread.joinable())
        {
            m_ioContext.stop();
            m_serviceThread.join();
        }
    }
    catch (std::exception& e)
    {
        CORE_ERROR(e.what());
    }
}

class Timeout : public Task
{
    std::shared_ptr<Task> m_task;
    std::chrono::system_clock::time_point m_executeAt;

  public:
    Timeout(std::shared_ptr<Task> task,
            std::chrono::system_clock::time_point executeAt)
        : m_task(task)
        , m_executeAt(executeAt)
    {
    }

    ~Timeout()
    {
        CORE_INFO("Stopped!!!!!!!!!!!!!");
    }

    Continuation run() override
    {
        if (std::chrono::system_clock::now() >= m_executeAt)
        {
            return m_task->run();
        }
        return RunAgain{};
    }
};

void TaskRunner::postDelayed(std::shared_ptr<Task> task, const std::chrono::milliseconds& delay)
{
    std::thread([this, isAlive = this->m_isAlive, task, delay] {
        std::this_thread::sleep_for(delay);
        if (isAlive->load())
            post(task);
    }).detach();
    // post(std::make_shared<Timeout>(task, std::chrono::system_clock::now() + delay));
}

template <class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

void TaskRunner::resolve(std::shared_ptr<Task> task)
{
    std::visit(overload{
                   [this, task](const Task::RunAgain&) {
                       post(task);
                   },
                   [this, task](const Task::RunAgainDelayed& run) {
                       postDelayed(task, run.delay);
                   },
                   [](const Task::Cancel&) {
                       /* Do nothing */
                   } },
               task->run());
}

} // namespace traits