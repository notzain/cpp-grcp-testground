#include "Logger.h"

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace
{
class FunctionFlag final : public spdlog::custom_flag_formatter
{
  public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override
    {
        std::string func = msg.source.funcname;
        if (const auto pos = func.find("<lambda_"); pos != std::string::npos)
        {
            func.replace(pos, func.size(), "<lambda>");
        }
        if (const auto begin = func.find_first_of("<"); begin != std::string::npos)
        {
            if (const auto end = func.find_last_of(">"); end != std::string::npos)
            {
                if (end - begin >= 20)
                    func.replace(begin, end, "<...>");
            }
        }
        dest.append(func.data(), func.data() + func.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<FunctionFlag>();
    }
};

static auto make_formatter(const std::string& format)
{
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<FunctionFlag>('!').set_pattern(format);
    return std::move(formatter);
}
} // namespace

namespace util
{
Logger::Logger()
    : m_logger(spdlog::create<spdlog::sinks::null_sink_st>("null_logger"))
{
    spdlog::init_thread_pool(8192, 1);

    m_dbg = spdlog::stderr_color_mt("dbg");
    m_dbg->set_formatter(make_formatter("[%T.%e] [%s:%#] %^%v%$"));
    m_dbg->set_level(spdlog::level::level_enum::trace);
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

    m_logger->set_formatter(make_formatter(
        fmt::format("[%^%-7l%$] [%Y-%m-%d %T.%e] {}%v",
                    [&]() {
                        if (options.verbose)
                        {
                            switch (options.sourceStyle)
                            {
                            case Logger::Options::SourceStyle::BaseFile: {
                                return "[%t | %s:%# - %!]\n";
                            }
                            break;
                            case Logger::Options::SourceStyle::RelativePath: {
                                return "[%t | %g:%# - %!]\n";
                            }
                            break;
                            }
                        }
                        return "[%s:%#] ";
                    }())));

    spdlog::register_logger(m_logger);
}

std::shared_ptr<spdlog::logger> Logger::log()
{
    return m_logger;
}

std::shared_ptr<spdlog::logger> Logger::debug()
{
    return m_dbg;
}

void Logger::setLevel(spdlog::level::level_enum level)
{
    m_logger->set_level(level);
}

} // namespace util