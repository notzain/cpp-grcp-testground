#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace core::util
{
class Logger
{
  public:
    static Logger &instance()
    {
        static Logger logger;
        return logger;
    }

    struct Options
    {
        std::string applicationName;
        std::string logPath;
    };

    void initialize(Options options);
    std::shared_ptr<spdlog::logger> log();
    std::shared_ptr<spdlog::logger> debug();
    void setLevel(spdlog::level::level_enum level);

  private:
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<spdlog::logger> m_debugLogger;
};
} // namespace core::util

#define CORE_TRACE(...) ::core::util::Logger::instance().log()->trace(__VA_ARGS__)
#define CORE_INFO(...) ::core::util::Logger::instance().log()->info(__VA_ARGS__)
#define CORE_WARN(...) ::core::util::Logger::instance().log()->warn(__VA_ARGS__)
#define CORE_ERROR(...) ::core::util::Logger::instance().log()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) ::core::util::Logger::instance().log()->critical(__VA_ARGS__)

#define CORE_DEBUG_INFO(...) SPDLOG_LOGGER_INFO(::core::util::Logger::instance().debug(), __VA_ARGS__)
#define CORE_DEBUG_WARN(...) SPDLOG_LOGGER_WARN(::core::util::Logger::instance().debug(), __VA_ARGS__)
#define CORE_DEBUG_ERROR(...) SPDLOG_LOGGER_ERROR(::core::util::Logger::instance().debug(), __VA_ARGS__)
