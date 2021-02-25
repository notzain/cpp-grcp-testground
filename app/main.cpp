#include <core/net/IPv4Range.h>
#include <core/net/NetworkInterface.h>

#include <core/net/ICMPScanner.h>
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

    auto interfaces = core::net::NetworkInterface::availableInterfaces();
    for (int i = 0; i < interfaces.size(); ++i)
    {
        CORE_INFO("{} - {}", interfaces[i]->getIPv4Address().toString(), interfaces[i]->getDesc());
    }

    if (auto networkInterface = core::net::NetworkInterface::byIp("192.168.178.52"))
    {
        core::net::ICMPScanner scanner(*networkInterface, core::net::SocketPool::defaultPool().createIcmpSocket());
        for (const auto ip : core::util::rangeOf<pcpp::IPv4Address>({ "192.168.178.1" }, { "192.168.178.10" }))
        {
            scanner.ping(ip.toString());
        }
    }

    std::getchar();
    return 1;

    if (const auto networkInterface = core::net::NetworkInterface::byIp("192.168.178.52"))
    {
        CORE_DEBUG_ERROR("Found interface");

        auto* device = networkInterface->device;
        if (!device->open())
        {
            CORE_ERROR("could not open {}", device->getName());
            return 1;
        };

        device->startCapture(
            [](pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie) {
                pcpp::Packet packet(pPacket);

                if (const auto* layer = packet.getLayerOfType<pcpp::ArpLayer>())
                {
                    if (boost::algorithm::contains(layer->toString(), "reply"))
                    {
                        CORE_DEBUG_INFO("{} - sender ip {} sender mac {}",
                                        layer->toString(),
                                        layer->getSenderIpAddr().toString(),
                                        layer->getSenderMacAddress().toString());
                    }
                }
            },
            nullptr);

        device->stopCapture();
    }
    else
    {
        CORE_WARN("{}", networkInterface.error());
        CORE_INFO("Available interfaces:");
        for (const auto* iface : core::net::NetworkInterface::availableInterfaces())
            CORE_INFO("\t{} - {}", iface->getName(), iface->getIPv4Address().toString());
    }

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
