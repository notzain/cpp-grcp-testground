#include <core/models/net/Device.h>
#include <core/net/socket/SocketPool.h>
#include <core/services/device_discovery/DeviceDiscoveryService.h>
#include <core/services/device_discovery/DeviceDiscoveryTaskBuilder.h>
#include <core/services/device_discovery/IcmpDeviceDiscoveryTask.h>
#include <core/traits/async/Resolver.h>
#include <core/util/logger/Logger.h>

#include <backward.hpp>
#include <chrono>
#include <docopt/docopt.h>
#include <iostream>

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help                   Show this screen.
    -p <port>, --port <port>    Port to run GRPC on [default: 5001]
    --log <file>                Set log file [default: my_log.txt]
    --verbose                   Set verbose logging
)";

class TimeoutTask : public traits::Task
{
    int i{ 0 };

  public:
    Continuation run() override
    {
        CORE_INFO("timeout {:%T}!", std::chrono::system_clock::now());
        if (++i >= 3)
            return traits::Task::Cancel{};

        return traits::Task::RunAgainDelayed{
            std::chrono::seconds(10)
        };
    }
};

int main(int argc, const char** argv)
{
    backward::SignalHandling sh;

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    util::Logger::instance().initialize({ "Embedded",
                                                args.at("--log").asString(),
                                                args.at("--verbose").asBool() });

    for (const auto& [key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }

    // auto runner = traits::TaskRunner();
    // runner.post(std::make_shared<TimeoutTask>());
    {
    auto runner = std::make_shared<traits::TaskRunner>();
    runner->post(std::make_shared<TimeoutTask>());
    }

    // net::DeviceDiscoveryService dds;
    // dds.addDiscoveryTask(
    //     net::DeviceDiscoveryTaskBuilder()
    //         .construct<net::IcmpDeviceDiscoveryTask>(net::SocketPool::defaultPool().createIcmpSocket())
    //         .withSuccessCallback([](const auto& result) {
    //             const auto timepoint = std::chrono::system_clock::to_time_t(result.completedAt);
    //             CORE_INFO("'{}' -> '{}' in {} arrived on {:%c}",
    //                       result.srcIp,
    //                       result.dstIp,
    //                       result.responseTime,
    //                       fmt::localtime(timepoint));
    //         })
    //         .withFailureCallback([](const auto& error) {
    //             CORE_INFO("'{}' -> '{}' failed",
    //                       error.srcIp,
    //                       error.dstIp);
    //         }));

        // dds.discover("192.168.178.1", "192.168.178.10");

        std::getchar();

        return 0;
}
