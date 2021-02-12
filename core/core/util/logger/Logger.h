#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace core::util
{
class Logger
{
  public:
    struct Options
    {
        std::string applicationName;
        std::string logPath;
    };

    void Initialize(Options options);
    std::shared_ptr<spdlog::logger> Debug();

    std::shared_ptr<spdlog::logger> operator()();

  private:
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<spdlog::logger> m_debugLogger;
};

static inline Logger logger;

} // namespace core::util

#define CORE_TRACE(...) ::core::util::logger()->trace(__VA_ARGS__)
#define CORE_INFO(...) ::core::util::logger()->info(__VA_ARGS__)
#define CORE_WARN(...) ::core::util::logger()->warn(__VA_ARGS__)
#define CORE_ERROR(...) ::core::util::logger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) ::core::util::logger()->critical(__VA_ARGS__)

#define CORE_DEBUG_INFO(...) SPDLOG_LOGGER_INFO(::core::util::logger.Debug(), __VA_ARGS__)
#define CORE_DEBUG_WARN(...) SPDLOG_LOGGER_WARN(::core::util::logger.Debug(), __VA_ARGS__)
#define CORE_DEBUG_ERROR(...) SPDLOG_LOGGER_ERROR(::core::util::logger.Debug(), __VA_ARGS__)
