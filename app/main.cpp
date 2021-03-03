#include <core/net/IPv4Range.h>
#include <core/net/NetworkInterface.h>

#include <core/net/scanner/ICMPScanner.h>
#include <core/net/socket/RawSocket.h>
#include <core/net/socket/SocketPool.h>
#include <core/traits/Printable.h>
#include <core/util/logger/Logger.h>
#include <core/util/type/WrapAround.h>

#include <ArpLayer.h>
#include <IpAddress.h>
#include <NetworkUtils.h>
#include <ProtocolType.h>

#include <boost/algorithm/string.hpp>
#include <docopt/docopt.h>

#include <iostream>
#include <string>
#include <thread>
#include <vector>

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help       Show this screen.
    -p <port>, --port <port>   Port to run GRPC on [default: 5001]
)";

int main(int argc, const char** argv)
{
    core::util::Logger::instance().initialize({ "Embedded",
                                                "my_log.txt" });

    {
        core::net::ICMPScanner scanner(core::net::SocketPool::defaultPool().createIcmpSocket());
        scanner.startReading();

        for (const auto ip : core::util::rangeOf<pcpp::IPv4Address>({ "192.168.178.1" }, { "192.168.178.10" }))
        {
            scanner.ping(ip.toString());
        }

        std::getchar();
    }

    return 1;

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    if (args.empty())
    {
        CORE_INFO("Use the application like so: \n{}", usage);
    }
    for (const auto& [key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }

    return 0;
}
