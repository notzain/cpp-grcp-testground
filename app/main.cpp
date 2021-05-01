#include <backward.hpp>
#include <chrono>
#include <core/models/net/Device.h>
#include <core/net/socket/SocketPool.h>
#include <core/net/socket/v2/IcmpSocket.h>
#include <core/services/device_discovery/DeviceDiscoveryService.h>
#include <core/services/device_discovery/DeviceDiscoveryTaskBuilder.h>
#include <core/services/device_discovery/IcmpDeviceDiscoveryTask.h>
#include <core/util/async/Resolver.h>
#include <core/util/async/ThreadContext.h>
#include <core/util/logger/Logger.h>
#include <docopt/docopt.h>
#include <iostream>
#include <thread>

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
    backward::SignalHandling sh;

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    util::Logger::instance().initialize({ "Embedded",
                                          args.at("--log").asString(),
                                          args.at("--verbose").asBool() });

    for (const auto& [key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }

    auto socket = net::SocketPool::defaultPool().createIcmpSocketv2();

    net::DeviceDiscoveryService dds;
    dds.addDiscoveryTask(
        net::DeviceDiscoveryTaskBuilder()
            .construct<net::IcmpDeviceDiscoveryTask>(net::SocketPool::defaultPool().createIcmpSocket())
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

    while (const auto c = std::getchar())
    {
        if (c == '\n')
            continue;
        if (c == 'q')
            break;

        if (c == 'c')
            dds.clearResults();
        else
            dds.discover("192.168.178.1", "192.168.178.10");
    }

    return 0;
}
