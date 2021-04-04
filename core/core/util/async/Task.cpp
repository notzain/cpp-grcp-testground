#include "Task.h"

namespace util
{

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
    if (m_serviceThread.joinable())
    {
        m_ioContext.stop();
        m_serviceThread.join();
    }
}

void TaskRunner::postDelayed(std::shared_ptr<Task> task, const std::chrono::milliseconds& delay)
{
    std::thread([this, isAlive = this->m_isAlive, task, delay] {
        std::this_thread::sleep_for(delay);
        if (isAlive->load())
            post(task);
    }).detach();
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
    std::visit(
        overload{
            [this, task](const Task::RunAgain&) {
                post(task);
            },
            [this, task](const Task::RunAgainDelayed& run) {
                postDelayed(task, run.delay);
            },
            [](const Task::End&) {
                /* Do nothing */
            } },
        task->run());
}
} // namespace util