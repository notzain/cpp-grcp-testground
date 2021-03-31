#include "fmt/chrono.h"
#include <core/models/net/Device.h>
#include <core/net/socket/SocketPool.h>
#include <core/services/device_discovery/DeviceDiscoveryService.h>
#include <core/services/device_discovery/DeviceDiscoveryTaskBuilder.h>
#include <core/services/device_discovery/IcmpDeviceDiscoveryTask.h>
#include <core/util/logger/Logger.h>

#include <docopt/docopt.h>
#include <iostream>
#include <spdlog/fmt/chrono.h>

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help                   Show this screen.
    -p <port>, --port <port>    Port to run GRPC on [default: 5001]
    --log <file>                Set log file [default: my_log.txt]
    --verbose                   Set verbose logging
)";

int main(int argc, const char** argv)
{
    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });

    core::util::Logger::instance().initialize({ "Embedded",
                                                args.at("--log").asString(),
                                                args.at("--verbose").asBool() });

    for (const auto& [key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }

    core::net::DeviceDiscoveryService dds;
    dds.addDiscoveryTask(
        core::net::DeviceDiscoveryTaskBuilder()
            .construct<core::net::IcmpDeviceDiscoveryTask>(core::net::SocketPool::defaultPool().createIcmpSocket())
            .withSuccessCallback([](const auto& result) {
                const auto timepoint = std::chrono::system_clock::to_time_t(result.completedAt);
                CORE_INFO("'{}' -> '{}' in {} arrived on {:%c}",
                          result.srcIp,
                          result.dstIp,
                          result.responseTime,
                          fmt::localtime(timepoint));
            })
            .withFailureCallback([](const auto& error) {
                CORE_INFO("'{}' -> '{}' failed",
                          error.srcIp,
                          error.dstIp);
            }));

    dds.discover("192.168.178.1", "192.168.178.10");

    std::getchar();

    return 0;
}
