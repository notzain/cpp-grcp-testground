#include <core/util/type/WrapAround.h>
#include <doctest/doctest.h>

#include <cstdint>
#include <limits>

TEST_SUITE("WrapAround")
{
    TEST_CASE("Defaults to numerical_limit of type for min and max")
    {
        SUBCASE("uint8_t")
        {
            REQUIRE(util::WrapAround<std::uint8_t>::Min == std::numeric_limits<std::uint8_t>::min());
            REQUIRE(util::WrapAround<std::uint8_t>::Max == std::numeric_limits<std::uint8_t>::max());
        }

        SUBCASE("int8_t")
        {
            REQUIRE(util::WrapAround<std::int8_t>::Min == std::numeric_limits<std::int8_t>::min());
            REQUIRE(util::WrapAround<std::int8_t>::Max == std::numeric_limits<std::int8_t>::max());
        }
    }
    TEST_CASE("Default constructor defaults to min value of type")
    {
        SUBCASE("uint8_t")
        {
            util::WrapAround<std::uint8_t> num;
            REQUIRE(num.value() == std::numeric_limits<std::uint8_t>::min());
        }

        SUBCASE("int16_t")
        {
            util::WrapAround<std::int16_t> num;
            REQUIRE(num.value() == std::numeric_limits<std::int16_t>::min());
        }

        SUBCASE("uint16_t")
        {
            util::WrapAround<std::uint16_t> num;
            REQUIRE(num.value() == std::numeric_limits<std::uint16_t>::min());
        }

        SUBCASE("uint8_t, custom min")
        {
            util::WrapAround<std::uint8_t, 16, 32> num;
            REQUIRE(num.value() == 16);
        }
    }

    TEST_CASE("Wraps to Min when Max has been reached")
    {
        util::WrapAround<std::uint8_t, 16, 32> num(32);
        REQUIRE(num.value() == 32);
        num++;
        REQUIRE(num.value() == 16);
    }

    TEST_CASE("Wraps to Max when Min has been reached")
    {
        util::WrapAround<std::uint8_t, 16, 32> num(16);
        REQUIRE(num.value() == 16);
        num--;
        REQUIRE(num.value() == 32);
    }

    TEST_CASE("Has ++ operator")
    {
        util::WrapAround<std::uint8_t> num(1);
        REQUIRE(num.value() == 1);
        num++;
        REQUIRE(num.value() == 2);
    }

    TEST_CASE("Has -- operator")
    {
        util::WrapAround<std::uint8_t> num(1);
        REQUIRE(num.value() == 1);
        num--;
        REQUIRE(num.value() == 0);
    }

    TEST_CASE("Has == operator")
    {
        util::WrapAround<std::uint8_t> num(1);
        REQUIRE(num == 1);
    }

    TEST_CASE("Has != operator")
    {
        util::WrapAround<std::uint8_t> num(1);
        REQUIRE(num != 2);
    }

    TEST_CASE("Has > operator")
    {
        util::WrapAround<std::uint8_t> num(1);
        REQUIRE(num > 0);
    }

    TEST_CASE("Has < operator")
    {
        util::WrapAround<std::uint8_t> num(1);
        REQUIRE(num < 2);
    }
}