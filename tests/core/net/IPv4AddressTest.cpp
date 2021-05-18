
#include <core/net/IPv4Address.h>
#include <core/util/ByteOrder.h>

#include <doctest/doctest.h>
#include <initializer_list>

TEST_SUITE("IPv4Address")
{
    TEST_CASE("Parsing IP address")
    {
        SUBCASE("Valid")
        {
            auto ipString = "192.168.13.5";
            std::array<std::uint8_t, 4> ipArrayHostOrder = { 192, 168, 13, 5 };
            std::array<std::uint8_t, 4> ipArrayNetworkOrder = { 5, 13, 168, 192 };
            std::initializer_list<std::uint8_t> ipListHostOrder = { 192, 168, 13, 5 };
            std::initializer_list<std::uint8_t> ipListNetworkOrder = { 5, 13, 168, 192 };
            auto ipNumber = 3232238853;

            REQUIRE(net::IPv4Address::parse(ipString).has_value());
            REQUIRE(net::IPv4Address::parse(ipString)->asString() == "192.168.13.5");

            REQUIRE(net::IPv4Address::parse(ipArrayHostOrder).has_value());
            REQUIRE(net::IPv4Address::parse(ipArrayHostOrder)->asString() == "192.168.13.5");

            REQUIRE(net::IPv4Address::parse(ipArrayNetworkOrder, ByteOrder::NetworkOrder).has_value());
            REQUIRE(net::IPv4Address::parse(ipArrayNetworkOrder, ByteOrder::NetworkOrder)->asString() == "192.168.13.5");

            REQUIRE(net::IPv4Address::parse(ipListHostOrder).has_value());
            REQUIRE(net::IPv4Address::parse(ipListHostOrder)->asString() == "192.168.13.5");

            REQUIRE(net::IPv4Address::parse(ipListNetworkOrder, ByteOrder::NetworkOrder).has_value());
            REQUIRE(net::IPv4Address::parse(ipListNetworkOrder, ByteOrder::NetworkOrder)->asString() == "192.168.13.5");

            REQUIRE(net::IPv4Address::parse(ipNumber).has_value());
            REQUIRE(net::IPv4Address::parse(ipNumber)->asString() == "192.168.13.5");
        }
        SUBCASE("Invalid")
        {
            auto ipString = "300.168.13.5";
            std::array<std::uint8_t, 3> ipArrayHostOrder = { 192, 168, 13 };
            std::array<std::uint8_t, 5> ipArrayNetworkOrder = { 5, 13, 168, 192, 0 };
            std::initializer_list<std::uint8_t> ipListHostOrder = { 192, 168, 13 };
            std::initializer_list<std::uint8_t> ipListNetworkOrder = { 5, 13, 168, 192, 0 };
            // Any uint32_t number is a valid IP address, there is no "invalid" number

            REQUIRE(!net::IPv4Address::parse(ipString).has_value());

            REQUIRE(!net::IPv4Address::parse(ipArrayHostOrder).has_value());

            REQUIRE(!net::IPv4Address::parse(ipArrayNetworkOrder).has_value());

            REQUIRE(!net::IPv4Address::parse(ipListHostOrder).has_value());

            REQUIRE(!net::IPv4Address::parse(ipListNetworkOrder).has_value());
        }
    }
}