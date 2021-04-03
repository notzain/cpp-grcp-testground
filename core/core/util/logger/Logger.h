#pragma once

#include <memory>
#include <spdlog/spdlog.h>

#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/ostr.h>

namespace util
{
class Logger
{
  public:
    static Logger& instance()
    {
        static Logger logger;
        return logger;
    }

    struct Options
    {
        std::string applicationName;
        std::string logPath;
        bool verbose = false;
        enum class SourceStyle
        {
            BaseFile,
            RelativePath
        } sourceStyle = SourceStyle::BaseFile;
    };

    void initialize(Options options);
    void setLevel(spdlog::level::level_enum level);

    std::shared_ptr<spdlog::logger> log();
    std::shared_ptr<spdlog::logger> dbg();

  private:
    Logger();
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<spdlog::logger> m_dbg;
};
} // namespace util

#define CORE_INFO(...) SPDLOG_LOGGER_INFO(::util::Logger::instance().log(), __VA_ARGS__)
#define CORE_DEBUG(...) SPDLOG_LOGGER_DEBUG(::util::Logger::instance().log(), __VA_ARGS__)
#define CORE_WARN(...) SPDLOG_LOGGER_WARN(::util::Logger::instance().log(), __VA_ARGS__)
#define CORE_ERROR(...) SPDLOG_LOGGER_ERROR(::util::Logger::instance().log(), __VA_ARGS__)
#define DBG(x)                                                                        \
    [&]() {                                                                           \
        SPDLOG_LOGGER_INFO(::util::Logger::instance().dbg(), "{} = {}", #x, x); \
        return x;                                                                     \
    }()
