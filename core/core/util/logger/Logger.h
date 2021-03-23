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
        enum class SourceStyle
        {
            BaseFile,
            RelativePath
        } sourceStyle;
    };

    void initialize(Options options);
    void setLevel(spdlog::level::level_enum level);

    std::shared_ptr<spdlog::logger> log();

  private:
    Logger();
    std::shared_ptr<spdlog::logger> m_logger;
};
} // namespace core::util

#define CORE_INFO(...) SPDLOG_LOGGER_INFO(::core::util::Logger::instance().log(), __VA_ARGS__)
#define CORE_WARN(...) SPDLOG_LOGGER_WARN(::core::util::Logger::instance().log(), __VA_ARGS__)
#define CORE_ERROR(...) SPDLOG_LOGGER_ERROR(::core::util::Logger::instance().log(), __VA_ARGS__)
