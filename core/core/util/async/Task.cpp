#include "Task.h"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/post.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include "core/util/helper/Variant.h"
#include "core/util/logger//Logger.h"

namespace util
{

TaskRunner::TaskRunner()
    : m_keepAlive(m_ioContext)
    , m_serviceThread(Thread::spawn("TaskRunner", [this] {
        m_ioContext.run();
    }))
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
    m_ioContext.stop();
}

void TaskRunner::postDelayed(std::shared_ptr<Task> task, const std::chrono::milliseconds& delay)
{
    auto timer = std::make_shared<boost::asio::deadline_timer>(m_ioContext, boost::posix_time::milliseconds(delay.count()));
    timer->async_wait([timer, this, isAlive = this->m_isAlive, task, delay](const boost::system::error_code&) {
        if (isAlive->load())
            post(task);
    });
}

void TaskRunner::resolve(std::shared_ptr<Task> task)
{
    matchVariant(
        task->run(),
        [this, task](const Task::RunAgain&) { post(task); },
        [this, task](const Task::RunAgainDelayed& run) { postDelayed(task, run.delay); },
        [](const Task::End&) { /* Do nothing */ });
}
} // namespace util