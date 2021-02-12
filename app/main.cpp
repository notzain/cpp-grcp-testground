
#include <core/net/NetworkInterface.h>
#include <core/services/ArpScanner.h>
#include <core/traits/Printable.h>
#include <core/util/logger/Logger.h>

#include <ArpLayer.h>
#include <IpAddress.h>
#include <NetworkUtils.h>
#include <ProtocolType.h>

#include <boost/algorithm/string.hpp>
#include <docopt/docopt.h>

#include <iostream>
#include <latch>
#include <string>
#include <thread>
#include <vector>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <core/net/external/zeroconf/zeroconf.hpp>

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help       Show this screen.
    -p <port>, --port <port>   Port to run GRPC on [default: 5001]
)";

void *get_in_addr(sockaddr_storage *sa)
{
    if (sa->ss_family == AF_INET)
        return &reinterpret_cast<sockaddr_in *>(sa)->sin_addr;

    if (sa->ss_family == AF_INET6)
        return &reinterpret_cast<sockaddr_in6 *>(sa)->sin6_addr;

    return nullptr;
}

int main(int argc, const char **argv)
{
    core::util::logger.Initialize({ .applicationName = "Embedded",
                                    .logPath = "my_log.txt" });

    std::latch latch(100);

    Zeroconf::SetLogCallback([](Zeroconf::LogLevel level, const std::string &message) {
        switch (level)
        {
        case Zeroconf::LogLevel::Error:
            CORE_ERROR(message);
            break;
        case Zeroconf::LogLevel::Warning:
            CORE_WARN(message);
            break;
        }
    });

    WSADATA wsa = { 0 };
    if (WSAStartup(0x202, &wsa) != 0)
    {
        return 1;
    }

    static const std::string MdnsQuery = "_http._udp.local";
    std::vector<Zeroconf::mdns_responce> result;
    bool st = Zeroconf::Resolve(MdnsQuery, /*scanTime*/ 10, &result);

    if (!st)
        std::cout << "MDNS query failed" << std::endl;
    else if (result.empty())
        std::cout << "No replies" << std::endl;
    else
        for (size_t i = 0; i < result.size(); i++)
        {
            auto &item = result[i];

            char buffer[INET6_ADDRSTRLEN + 1] = { 0 };
            inet_ntop(item.peer.ss_family, get_in_addr(&item.peer), buffer, INET6_ADDRSTRLEN);
            std::cout << "Peer: " << buffer << std::endl;

            if (!item.records.empty())
            {
                std::cout << "Answers:" << std::endl;
                for (size_t j = 0; j < item.records.size(); j++)
                {
                    auto &rr = item.records[j];
                    std::cout << " " << j;
                    std::cout << ": type ";
                    switch (rr.type)
                    {
                    case 1:
                        std::cout << "A";
                        break;
                    case 12:
                        std::cout << "PTR";
                        break;
                    case 16:
                        std::cout << "TXT";
                        break;
                    case 28:
                        std::cout << "AAAA";
                        break;
                    case 33:
                        std::cout << "SRV";
                        break;
                    default:
                        std::cout << rr.type;
                    }
                    std::cout << ", size " << rr.len;
                    std::cout << ", " << rr.name << std::endl;
                }
            }

            if (i != result.size() - 1)
                std::cout << "------" << std::endl;
        }

    if (const auto networkInterface = core::net::NetworkInterface::GetByIp("192.168.178.52"))
    {
        CORE_DEBUG_ERROR("Found interface");

        auto *device = networkInterface->device;
        if (!device->open())
        {
            CORE_ERROR("could not open {}", device->getName());
            return 1;
        };

        device->startCapture([](pcpp::RawPacket *pPacket, pcpp::PcapLiveDevice *pDevice, void *userCookie) {
            pcpp::Packet packet(pPacket);

            if (const auto *layer = packet.getLayerOfType<pcpp::ArpLayer>())
            {
                if (boost::algorithm::contains(layer->toString(), "reply"))
                {
                    CORE_DEBUG_INFO("{} - sender ip {} sender mac {}", layer->toString(), layer->getSenderIpAddr().toString(), layer->getSenderMacAddress().toString());
                    static_cast<std::latch *>(userCookie)->count_down();
                }
            }
        },
                             &latch);

        latch.wait();
        device->stopCapture();
    }
    else
    {
        CORE_WARN("{}", networkInterface.error());
        CORE_INFO("Available interfaces:");
        for (const auto *iface : core::net::NetworkInterface::GetAvailableInterfaces())
            CORE_INFO("\t{} - {}", iface->getName(), iface->getIPv4Address().toString());
    }

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    if (args.empty())
    {
        CORE_INFO("Use the application like so: \n{}", usage);
    }
    for (const auto &[key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }

    return 0;
}
