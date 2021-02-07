#include "Logger.h"
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace core::util
{

void Logger::Initialize(Options options)
{
    spdlog::init_thread_pool(8192, 1);

    std::vector<spdlog::sink_ptr> sinks{
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::move(options.logPath))};

    m_logger = std::make_shared<spdlog::async_logger>(
        std::move(options.applicationName),
        sinks.begin(),
        sinks.end(),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);

    spdlog::register_logger(m_logger);
}

std::shared_ptr<spdlog::logger> Logger::operator()()
{
    return m_logger;
}
} // namespace core::util