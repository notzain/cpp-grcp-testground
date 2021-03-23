#include <core/models/net/Device.h>
#include <core/net/socket/SocketPool.h>
#include <core/services/device_discovery/DeviceDiscoveryService.h>
#include <core/util/logger/Logger.h>

#include <docopt/docopt.h>

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help       Show this screen.
    -p <port>, --port <port>   Port to run GRPC on [default: 5001]
)";

int main(int argc, const char** argv)
{
    core::util::Logger::instance().initialize({ "Embedded",
                                                "my_log.txt",
                                                core::util::Logger::Options::SourceStyle::BaseFile });

    core::models::Device device("192.168.1.1", "FF:FF:FF:FF:FF:FF");
    CORE_INFO(device);
    CORE_INFO(device.serialize().dump());

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    if (args.empty())
    {
        CORE_INFO("Use the application like so: \n{}", usage);
    }

    for (const auto& [key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }

    core::net::DeviceDiscoveryService dds;
    dds.addDiscoveryTask<core::net::IcmpDeviceDiscoveryTask>(core::net::SocketPool::defaultPool().createIcmpSocket());

    dds.runOnce("192.168.178.1", "192.168.178.10");

    std::getchar();

    return 0;
}
