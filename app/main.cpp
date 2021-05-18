#include <backward.hpp>
#include <chrono>
#include <core/models/net/Device.h>
#include <core/net/IPv4Address.h>
#include <core/net/MacAddress.h>
#include <core/net/socket/SocketPool.h>
#include <core/net/socket/v2/IcmpSocket.h>
#include <core/services/device_discovery/DeviceDiscoveryService.h>
#include <core/services/device_discovery/DeviceDiscoveryTaskBuilder.h>
#include <core/services/device_discovery/IcmpDeviceDiscoveryTask.h>
#include <core/util/Enum.h>
#include <core/util/Result.h>
#include <core/util/Thread.h>
#include <core/util/async/Resolver.h>
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
    util::Logger::instance().initialize({ "Embedded", args.at("--log").asString(), args.at("--verbose").asBool() });

    auto ip = net::IPv4Address::parse("192.168.17.69")
                  .value_or(net::IPv4Address::zero());
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
                  .asInt());

    CORE_INFO(net::MacAddress::parse("f3:08:7d:2f:5c:e0")
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

    net::DeviceDiscoveryService dds;
    dds.addDiscoveryTask(
        net::DeviceDiscoveryTaskBuilder()
            .construct<net::IcmpDeviceDiscoveryTask>(net::SocketPool::defaultPool().createRawSocket("enp0s3"))
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
            dds.discover("192.168.178.1", "192.168.178.2");
    }

    return 0;
}

/*
#include <EthLayer.h>
#include <IPv4Layer.h>
#include <IcmpLayer.h>
#include <IpAddress.h>
#include <Packet.h>
#include <PcapFileDevice.h>
#include <SystemUtils.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netpacket/packet.h>

#include <cstdint>
#include <deque>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <utility>

#include <boost/asio.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/tokenizer.hpp>

template <typename OutputIterator>
OutputIterator tokenize_and_copy(
    std::string const& address,
    OutputIterator output_iterator,
    bool const reverse_byte_order = false)
{
    using SeparatorType = boost::char_separator<char>;
    using TokenizerType = boost::tokenizer<SeparatorType>;

    if (address.length() != 17)
    {
        throw std::runtime_error("the given address is malformed.");
    }

    SeparatorType const separator(":");
    TokenizerType const tokenizer(address, separator);

    auto const transform_function = [](std::string const& string) -> int {
        return std::stoi(string, nullptr, 16);
    };
    auto const range = tokenizer |
                       boost::adaptors::transformed(
                           transform_function);

    std::deque<char> result;
    if (reverse_byte_order)
    {
        boost::copy(range, std::front_inserter(result));
    }
    else
    {
        boost::copy(range, std::back_inserter(result));
    }

    if (result.size() != 6)
    {
        throw std::runtime_error("the given address is malformed.");
    }

    return boost::copy(result, output_iterator);
}

template <typename OutputIterator>
void generate_ethernet_header(
    std::string const& source,
    std::string const& destination,
    std::uint32_t const& type,
    OutputIterator output_iterator,
    bool const reverse_byte_order = false)
{
    output_iterator = tokenize_and_copy(destination, output_iterator, reverse_byte_order);
    output_iterator = tokenize_and_copy(source, output_iterator, reverse_byte_order);

    const char type_array[] = { static_cast<char>(type & 0x00FF), static_cast<char>(type >> 8) };
    *output_iterator++ = type_array[(reverse_byte_order ? 0 : 1)];
    *output_iterator++ = type_array[(reverse_byte_order ? 1 : 0)];
}

using EndpointType = boost::asio::generic::raw_protocol::endpoint;

EndpointType create_ethernet_endpoint(std::string const& ifname)
{
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_ifindex = if_nametoindex(ifname.c_str());

    return std::move(
        EndpointType(reinterpret_cast<EndpointType::data_type*>(&sll), sizeof(sll)));
}

int main()
{
    {
        sockaddr_ll sockaddr = sockaddr_ll();

        sockaddr.sll_family = PF_PACKET;
        sockaddr.sll_protocol = htons(ETH_P_ALL);
        // is the interface index of the interface
        sockaddr.sll_ifindex = if_nametoindex("enp0s3");
        sockaddr.sll_hatype = 1;

        boost::asio::io_service io_service;
        boost::asio::io_service::work serviceKeepAlive(io_service);

        boost::asio::generic::raw_protocol::socket socket(io_service, boost::asio::generic::raw_protocol(PF_PACKET, SOCK_RAW));

        // socket.bind(boost::asio::generic::raw_protocol::endpoint(&sockaddr, sizeof(sockaddr)));

        boost::asio::generic::raw_protocol::socket::receive_buffer_size option;
        socket.get_option(option);

        std::string in_buffer(option.value(), '\0');

        auto rep = boost::asio::generic::raw_protocol::endpoint();
        while (false)
        {
            socket.receive_from(boost::asio::buffer(&in_buffer[0], in_buffer.size()), rep);
            // Source addr
            printf("%d.%d.%d.%d - ", (unsigned char)(in_buffer[26]), (unsigned char)(in_buffer[27]), (unsigned char)(in_buffer[28]), (unsigned char)(in_buffer[29]));
            // Destination addr
            printf("%d.%d.%d.%d\n", (unsigned char)(in_buffer[30]), (unsigned char)(in_buffer[31]), (unsigned char)(in_buffer[32]), (unsigned char)(in_buffer[33]));
        }

        while (true)
        {
            socket.async_receive_from(boost::asio::buffer(&in_buffer[0], in_buffer.size()), rep, [&](const boost::system::error_code& ec, std::size_t length) {
                // Source addr
                printf("%d.%d.%d.%d - ", (unsigned char)(in_buffer[26]), (unsigned char)(in_buffer[27]), (unsigned char)(in_buffer[28]), (unsigned char)(in_buffer[29]));
                // Destination addr
                printf("%d.%d.%d.%d\n", (unsigned char)(in_buffer[30]), (unsigned char)(in_buffer[31]), (unsigned char)(in_buffer[32]), (unsigned char)(in_buffer[33]));

                socket.async_receive_from(boost::asio::buffer(&in_buffer[0], in_buffer.size()), rep, [&](const boost::system::error_code& ec, std::size_t length) {
                    // Source addr
                    printf("%d.%d.%d.%d - ", (unsigned char)(in_buffer[26]), (unsigned char)(in_buffer[27]), (unsigned char)(in_buffer[28]), (unsigned char)(in_buffer[29]));
                    // Destination addr
                    printf("%d.%d.%d.%d\n", (unsigned char)(in_buffer[30]), (unsigned char)(in_buffer[31]), (unsigned char)(in_buffer[32]), (unsigned char)(in_buffer[33]));
                });
            });

            io_service.run();
            return 0;
        }
    }

    using IoServiceType = boost::asio::io_service;
    using ProtocolType = boost::asio::generic::raw_protocol;
    using SocketType = boost::asio::basic_raw_socket<ProtocolType>;

    IoServiceType io_service;
    SocketType socket(io_service);
    ProtocolType protocol(AF_PACKET, SOCK_RAW);

    boost::asio::streambuf buffer;
    std::ostream stream(&buffer);

    // // イーサネットヘッダを書き込む
    // generate_ethernet_header(
    //     "12:34:56:78:90:ab",
    //     "FF:FF:FF:FF:FF:FF",
    //     0x1001,
    //     std::ostream_iterator<char>(stream));

    // // データ部分を書き込む
    // unsigned char const deadbeef[] = { 0xde, 0xad, 0xbe, 0xef };
    // stream << "Hello, Ojisan!!!"
    //        << reinterpret_cast<char const*>(deadbeef);

    boost::system::error_code error;

    std::vector<uint8_t> msg = { 90, 0x5c, 0x44, 0x4b, 0x0f, 0x9b, 0x08, 0x00, 0x27, 0x21, 0xf8, 0x3c, 0x08, 0x00, 0x45, 0x00, 0x00, 0x54, 0xcb, 0xee, 0x40, 0x00, 0x40, 0x01, 0x88, 0xc2, 0xc0, 0xa8, 0xb2, 0xa5, 0xc0, 0xa8, 0xb2, 01, 0x08, 0x00, 0xff, 0x5c, 0x00, 0x02, 0x00, 0x01, 0x56, 0x2a, 0x97, 0x60, 0x00, 0x00, 0x00, 0x00, 0x3d, 0x42, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };

    pcpp::Packet packet;
    pcpp::EthLayer ethLayer("08:00:27:21:f8:3c", "ff:ff:ff:ff:ff:ff");
    pcpp::IPv4Layer ipLayer(pcpp::IPv4Address("192.168.178.165"), pcpp::IPv4Address("192.168.178.1"));
    ipLayer.getIPv4Header()->ipId = htons(2000);
    ipLayer.getIPv4Header()->timeToLive = 64;
    pcpp::IcmpLayer icmpLayer;
    icmpLayer.setEchoRequestData(std::hash<std::thread::id>()(std::this_thread::get_id()),
                                 0,
                                 0,
                                 (const std::uint8_t*)"WHAT",
                                 5);

    packet.addLayer(&ethLayer);
    packet.addLayer(&ipLayer);
    packet.addLayer(&icmpLayer);
    packet.computeCalculateFields();

    for (int i = 0; i < packet.getRawPacket()->getRawDataLen(); ++i)
        stream << packet.getRawPacket()->getRawData()[i];
    try
    {
        socket.open(protocol);
        socket.bind(create_ethernet_endpoint("enp0s3"));
        auto const size = socket.send(
            buffer.data());
    }
    catch (std::exception& e)
    {
        std::cerr << e.what();
    }

    std::cout << error << std::endl;
    return 0;
}
*/

/*
#include <arpa/inet.h>
#include <array>
#include <boost/asio/generic/raw_protocol.hpp>
#include <cstring>
#include <iostream>
#include <linux/filter.h>
#include <linux/if_ether.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int _get_MAC(void)
{
    int sock, sockfd, n, cnt;
    char buffer[2048];
    unsigned char *iphead, *ethhead;
    struct ether_addr ether;

    if ((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0)
    {
        perror("socket");
        exit(1);
    }

    while (1)
    {
        if (n = recvfrom(sock, buffer, 2048, 0, NULL, NULL) == -1)
        {
            perror("recvfrom");
            close(sock);
            exit(1);
        }

        ethhead = (unsigned char*)buffer;

        if (ethhead != NULL)
        {
            iphead = (unsigned char*)(buffer + 14); // Skip Ethernet header
            printf("Protocolo   (UDP=11): %02x Hex\n", iphead[9]);
            printf("\n--------------------------------------"
                   "\nMAC destino (server): "
                   "%02x:%02x:%02x:%02x:%02x:%02x\n",
                   ethhead[0],
                   ethhead[1],
                   ethhead[2],
                   ethhead[3],
                   ethhead[4],
                   ethhead[5]);
            printf("MAC origen  (CAL30x): "
                   "%02x:%02x:%02x:%02x:%02x:%02x\n",
                   ethhead[6],
                   ethhead[7],
                   ethhead[8],
                   ethhead[9],
                   ethhead[10],
                   ethhead[11]);

            if (*iphead == 0x45)
            { // Double check for IPv4 and no options present
                printf("IP destino  (server): %d.%d.%d.%d\n",
                       iphead[12],
                       iphead[13],
                       iphead[14],
                       iphead[15]);
                printf("IP origen   (CAL30x): %d.%d.%d.%d\n",
                       iphead[16],
                       iphead[17],
                       iphead[18],
                       iphead[19]);
            }
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    _get_MAC();
    return 0;
}
*/