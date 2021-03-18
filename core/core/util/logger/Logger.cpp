#include "Logger.h"
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace core::util
{
Logger::Logger()
    : m_logger(spdlog::create<spdlog::sinks::null_sink_st>("null_logger"))
{
    spdlog::init_thread_pool(8192, 1);
}

void Logger::initialize(Options options)
{
    std::vector<spdlog::sink_ptr> sinks{
        std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::move(options.logPath))
    };

    m_logger = std::make_shared<spdlog::async_logger>(
        std::move(options.applicationName),
        sinks.begin(),
        sinks.end(),
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block);

    m_logger->set_pattern("[%Y-%m-%d %T.%e] [%n] [%^%L%$] [%t | %s:%# - %!] %v");

    spdlog::register_logger(m_logger);
}

std::shared_ptr<spdlog::logger> Logger::log()
{
    return m_logger;
}

void Logger::setLevel(spdlog::level::level_enum level)
{
    m_logger->set_level(level);
}

} // namespace core::util