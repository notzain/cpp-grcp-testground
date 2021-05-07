#include "Thread.h"

#include <boost/assert.hpp>
#include <fstream>
#include <spdlog/fmt/fmt.h>
#include <string>

#include <pthread.h>

static std::string_view executableName()
{
    static std::string name = []() -> std::string {
        std::string sp;
        std::ifstream("/proc/self/comm") >> sp;
        return sp;
    }();

    return name;
}

namespace util
{
Thread::~Thread()
{
    requestStop();

    if (m_thread.joinable())
        m_thread.join();
}

void Thread::requestStop()
{
    m_stopToken.store(Thread::Stop);
}

void Thread::setName(std::string_view name)
{
    const auto threadName = fmt::format("{} {}", executableName(), name);
    BOOST_ASSERT_MSG(threadName.length() <= 16, "Thread name cannot exceed 16 characters");

    pthread_setname_np(m_thread.native_handle(), threadName.c_str());
}

} // namespace util