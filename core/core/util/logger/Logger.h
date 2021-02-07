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
    std::shared_ptr<spdlog::logger> operator()();

  private:
    std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace core::util

static inline core::util::Logger logger;
