#pragma once

#include <memory>
#include <type_traits>

// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
// clang-format on

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
    std::shared_ptr<spdlog::logger> debug();

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

#ifndef CORE_DBG_DISABLED
#include "LogHelper.h"

#define dbg(_dbg_var)                                                                                                                                   \
    [&_dbg_var]() {                                                                                                                                     \
        if constexpr (_detail::is_mapping_v<std::decay<decltype(_dbg_var)>::type>)                                                                      \
            SPDLOG_LOGGER_INFO(::util::Logger::instance().debug(), "{} = [{}]", #_dbg_var, MapPrinter<std::decay<decltype(_dbg_var)>::type>(_dbg_var)); \
        else if constexpr (_detail::is_iterable_v<std::decay<decltype(_dbg_var)>::type>)                                                                \
            SPDLOG_LOGGER_INFO(::util::Logger::instance().debug(), "{} = [{}]", #_dbg_var, fmt::join(_dbg_var, ", "));                                  \
        else                                                                                                                                            \
            SPDLOG_LOGGER_INFO(::util::Logger::instance().debug(), "{} = {}", #_dbg_var, _dbg_var);                                                     \
        return _dbg_var;                                                                                                                                \
    }()

#define dbg_if(cond, _dbg_var)                                                                                                                                       \
    [&]() {                                                                                                                                                          \
        if (!(cond))                                                                                                                                                 \
            return _dbg_var;                                                                                                                                         \
        if constexpr (_detail::is_mapping_v<std::decay<decltype(_dbg_var)>::type>)                                                                                   \
            SPDLOG_LOGGER_INFO(::util::Logger::instance().debug(), "{} -> {} = [{}]", #cond, #_dbg_var, MapPrinter<std::decay<decltype(_dbg_var)>::type>(_dbg_var)); \
        else if constexpr (_detail::is_iterable_v<std::decay<decltype(_dbg_var)>::type>)                                                                             \
            SPDLOG_LOGGER_INFO(::util::Logger::instance().debug(), "{} -> {} = [{}]", #cond, #_dbg_var, fmt::join(_dbg_var, ", "));                                  \
        else                                                                                                                                                         \
            SPDLOG_LOGGER_INFO(::util::Logger::instance().debug(), "{} -> {} = {}", #cond, #_dbg_var, _dbg_var);                                                     \
        return _dbg_var;                                                                                                                                             \
    }()
#else
#define dbg(_dbg_var) [&]() { return _dbg_var; }()
#define dbg_if(cond, _dbg_var) [&]() { return _dbg_var; }()
#endif
