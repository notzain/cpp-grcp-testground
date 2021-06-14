#pragma once

#include "core/traits/Printable.h"
#include "core/util/Enum.h"
#include "core/util/Result.h"

#include <cstdint>
#include <snmp_pp/oid.h>
#include <string_view>
#include <variant>

namespace net
{
class Oid : public traits::Printable<Oid>
{
  public:
    enum class Type
    {
        Integer32,
        UInteger32,
        OctetString,
        Oid,
        Counter64,
        TimeTicks,
        Opaque,
        Integer = Integer32,
        Counter32 = UInteger32,
        Gauge32 = UInteger32,
    };

  private:
    Snmp_pp::Oid m_oid;

    template <Type T, typename V>
    struct Value : public traits::Printable<Value<T, V>>
    {
        Type type = T;
        V value;

        Value(V val)
            : value(std::move(val))
        {
        }

        std::string toString() const override { return fmt::format("{} ({})", value, type); };
    };

  public:
    static Result<Oid> parse(std::string_view oid);

    // clang-format off
#define DefineOid(oidName, oidString) static const Oid& oidName() { static Oid oid = *parse(oidString); return oid; }
  DefineOid(system     , "1.3.6.1.2.1.1");
  DefineOid(sysDescr   , "1.3.6.1.2.1.1.1.0");
  DefineOid(sysUpTime  , "1.3.6.1.2.1.1.3.0");
  DefineOid(sysContact , "1.3.6.1.2.1.1.4.0");
  DefineOid(sysName    , "1.3.6.1.2.1.1.5.0");
  DefineOid(sysLocation, "1.3.6.1.2.1.1.6.0");

  DefineOid(ifTable    , "1.3.6.1.2.1.2.2");
#undef DefineOid
    // clang-format on

    std::string_view asDottedString() const;
    std::string toString() const override;

    bool operator==(const Oid& other) const { return m_oid == other.m_oid; }
    bool operator!=(const Oid& other) const { return !(*this == other); }
    bool operator<(const Oid& other) const { return m_oid < other.m_oid; }
    bool operator>(const Oid& other) const { return other < *this; }
    bool operator<=(const Oid& other) const { return !(other < *this); }
    bool operator>=(const Oid& other) const { return !(*this < other); }

    using Integer32 = Value<Type::Integer32, std::int32_t>;
    using UInteger32 = Value<Type::UInteger32, std::uint32_t>;
    using OctetString = Value<Type::OctetString, std::string>;
    using OidType = Value<Type::Oid, Oid>;
    using Counter64 = Value<Type::Counter64, std::uint64_t>;
    using TimeTicks = Value<Type::TimeTicks, std::uint32_t>;
    using Opaque = Value<Type::Opaque, std::string>;
    using Integer = Integer32;
    using Counter32 = UInteger32;
    using Gauge32 = UInteger32;

  private:
    Oid(Snmp_pp::Oid oid)
        : m_oid(std::move(oid))
    {
    }
};

using OidValue = std::variant<Oid::Integer32,
                              Oid::UInteger32,
                              Oid::OctetString,
                              Oid::OidType,
                              Oid::Counter64,
                              Oid::TimeTicks,
                              Oid::Opaque>;

} // namespace net

REGISTER_ENUM(net::Oid::Type);