#include <backward.hpp>
#include <chrono>
#include <core/models/net/Device.h>
#include <core/net/IPv4Address.h>
#include <core/net/MacAddress.h>
#include <core/net/snmp/Oid.h>
#include <core/net/snmp/SnmpClient.h>
#include <core/net/snmp/SnmpTarget.h>
#include <core/net/socket/SocketPool.h>
#include <core/net/socket/v2/IcmpSocket.h>
#include <core/repo/RepositoryRegistry.h>
#include <core/repo/device/DeviceRepository.h>
#include <core/services/device_discovery/DeviceDiscoveryService.h>
#include <core/services/device_discovery/DeviceDiscoveryTaskBuilder.h>
#include <core/services/device_discovery/icmp/IcmpDeviceDiscoveryTask.h>
#include <core/services/snmp_service/SnmpService.h>
#include <core/util/Enum.h>
#include <core/util/Result.h>
#include <core/util/Sequence.h>
#include <core/util/Thread.h>
#include <core/util/async/TaskContext.h>
#include <core/util/logger/Logger.h>
#include <docopt/docopt.h>
#include <iostream>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <string>
#include <sys/socket.h>
#include <thread>

#include "MyIcmpDeviceDiscoveryTask.h"
#include "core/net/snmp/Oid.h"
#include "core/util/IterResult.h"

const auto usage = R"(GRPC Application.
Usage: app [options]

Options:
    -h --help                   Show this screen.
    -p <port>, --port <port>    Port to run GRPC on [default: 5001]
    --log <file>                Set log file [default: my_log.txt]
    --verbose                   Set verbose logging
)";

int main(int argc, char** argv)
{
    backward::SignalHandling sh;

    const auto args = docopt::docopt(usage, { argv + 1, argv + argc });
    util::Logger::instance().initialize({ "Embedded",
                                          args.at("--log").asString(),
                                          args.at("--verbose").asBool() });

    auto snmp = *net::SnmpClient::create();
    auto snmpService = service::SnmpService(*snmp);

    auto repoRegistry = repo::RepositoryRegistry();

    auto deviceRepositoryOrError = repoRegistry.registerRepository<repo::DeviceRepository>();
    auto deviceRepo = *deviceRepositoryOrError;

    auto ip = net::IPv4Address::parse("192.168.17.69")
                  .value_or(net::IPv4Address::zero());
    auto device = models::Device(ip, net::MacAddress::broadcast());

    CORE_INFO("{}", fmt::join(net::Oid::systemTree(), ", "));

    for (const auto& [type, name] : Enum::entries<net::Oid::Type>())
    {
        CORE_INFO("{} {}", type, name);
    }
    CORE_INFO(ip);
    std::array<std::uint8_t, 4> bytes = { 192, 168, 23, 63 };
    CORE_INFO(net::IPv4Address::parse({ 192, 168, 66, 23 })
                  .value_or(net::IPv4Address::zero()));
    CORE_INFO(net::IPv4Address::parse(bytes)
                  .value_or(net::IPv4Address::zero()));
    CORE_INFO(net::IPv4Address::parse(3232239904)
                  .value_or(net::IPv4Address::zero()));
    CORE_INFO(net::IPv4Address::parse(3232239904)
                  .value_or(net::IPv4Address::zero())
                  .toInt());

    CORE_INFO("{}",
              fmt::join(net::IPv4Address::parse(3232239904)
                            .value_or(net::IPv4Address::zero())
                            .toBytes(),
                        ", "));
    CORE_INFO("{}",
              fmt::join(net::IPv4Address::parse(3232239904)
                            .value_or(net::IPv4Address::zero())
                            .toBytes(ByteOrder::NetworkOrder),
                        ", "));

    CORE_INFO(net::MacAddress::parse("f3 08:7d:2f:5c:e0")
                  .value_or(net::MacAddress::zero()));
    CORE_INFO(net::MacAddress::parse("f3 08 7d 2f 5c e0")
                  .value_or(net::MacAddress::zero()));
    CORE_INFO(net::MacAddress::parse(267217785543904)
                  .value_or(net::MacAddress::zero()));
    CORE_INFO(net::MacAddress::parse("14:D7:D8:C8:A7:77")
                  .value_or(net::MacAddress::zero()));
    CORE_INFO(net::MacAddress::parse("14:D7:D8:C8:A7:121")
                  .value_or(net::MacAddress::zero()));
    CORE_INFO(net::MacAddress::parse("14:D7:D8:C8:A7:77:33")
                  .value_or(net::MacAddress::zero()));

    for (const auto& [key, value] : args)
    {
        CORE_INFO("{} - {}", key, value);
    }
    auto range = std::vector{ 3, 5, 2, 18, 328, 10 };

    for (const auto& [index, value] : util::rangeIndexed(range))
    {
        CORE_INFO("value = {}, index = {}", value, index);
    }
    for (const auto& ip : util::rangeOf(*net::IPv4Address::parse("192.168.178.1"), *net::IPv4Address::parse("192.168.178.5")))
    {
        CORE_INFO(ip);
    }

    CORE_INFO("{}", fmt::join(util::rangeOf(1, 5), ", "));
    CORE_INFO("{}", fmt::join(util::rangeOf(1, util::Inclusive(5)), ", "));
    CORE_INFO("{}", fmt::join(util::rangeOf(util::Exclusive(1), 5), ", "));

    // for (const auto& mac : util::rangeOf(net::MacAddress::zero(), *net::MacAddress::parse("00:00:00:00:01:FF")))
    // {
    //     CORE_INFO(mac);
    // }

    net::DeviceDiscoveryService dds;
    dds.addDiscoveryTask(
        net::DeviceDiscoveryTaskBuilder()
            .construct<app::MyIcmpDeviceDiscoveryTask>(net::SocketPool::defaultPool().createRawSocket("enp0s3"), *deviceRepo)
            .withSuccessCallback([&](const auto& result) {
                const auto timepoint = std::chrono::system_clock::to_time_t(result.completedAt);
                CORE_INFO("'{} ({})' -> '{} ({})' in {} arrived on {:%c}",
                          result.srcIp,
                          result.srcMac.value_or(net::MacAddress::zero()),
                          result.dstIp,
                          result.dstMac.value_or(net::MacAddress::zero()),
                          result.responseTime,
                          fmt::localtime(timepoint));
                if (result.dstMac)
                {
                    if (auto deviceOrError = deviceRepo->read(*result.dstMac))
                    {
                        auto device = deviceOrError.value();
                        device->setResponseTime(result.responseTime);
                    }
                    else
                    {
                        auto device = std::make_shared<models::Device>(result.dstIp, *result.dstMac);
                        device->setResponseTime(result.responseTime);
                        CORE_INFO("{}", device->serialize().dump());
                        deviceRepo->create(*result.dstMac, std::move(device));
                    }
                }
            })
            .withFailureCallback([](const auto& error) {
                CORE_INFO("'{}' failed",
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
        {
            snmpService.requestSystem(net::IPv4Address::localhost(), 1613);
            // deviceRepo->iterate([&snmpService](const models::Device& device) {
            // });
            // dds.discover(*net::IPv4Address::parse("192.168.178.1"), *net::IPv4Address::parse("192.168.178.2"));
        }
    }

    return 0;
}
