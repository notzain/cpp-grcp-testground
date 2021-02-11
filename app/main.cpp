#include <IpAddress.h>
#include <core/net/NetworkInterface.h>
#include <core/services/ArpScanner.h>
#include <core/traits/Printable.h>
#include <core/util/logger/Logger.h>

#include <Logger.h>
#include <NetworkUtils.h>

#include <docopt/docopt.h>

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help       Show this screen.
    -p <port>, --port <port>   Port to run GRPC on [default: 5001]
)";

int main(int argc, const char **argv)
{
    logger.Initialize({ .applicationName = "Embedded",
                        .logPath = "my_log.txt" });

    if (const auto networkInterface = core::net::NetworkInterface::GetByIp("192.168.178.52"))
    {
        logger()->info("Found interface");

        auto *device = networkInterface->device;
        if (!device->open())
        {
            logger()->error("could not open {}", device->getName());
            return 1;
        };
        double responseTime = 0;
        logger()->info("Opened interface, starting scan");

        auto result = pcpp::NetworkUtils::getInstance().getMacAddress(
            pcpp::IPv4Address("192.168.178.1"),
            device,
            responseTime);

        logger()->info("{} took {} ms", result.toString(), responseTime);
    }
    else
    {
        logger()->warn("{}", networkInterface.error());
        logger()->info("Available interfaces:");
        for (const auto *iface : core::net::NetworkInterface::GetAvailableInterfaces())
            logger()->info("\t{} - {}", iface->getName(), iface->getIPv4Address().toString());
    }

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    if (args.empty())
    {
        logger()->info("Use the application like so: \n{}", usage);
    }
    for (const auto &[key, value] : args)
    {
        logger()->info("{} - {}", key, value);
    }

    return 0;
}
