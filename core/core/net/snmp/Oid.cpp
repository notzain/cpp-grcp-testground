#include "Oid.h"

namespace net
{
Result<Oid> Oid::parse(std::string_view oid)
{
    Snmp_pp::Oid snmpOid(oid.data());
    if (!snmpOid.valid())
        return Error(ErrorType::InvalidArgument);

    return Oid(std::move(snmpOid));
}

const Oid& Oid::sysDescr()
{
    static Oid sysDescr = *parse("1.3.6.1.2.1.1.1.0");
    return sysDescr;
}

const Oid& Oid::sysUpTime()
{
    static Oid sysUpTime = *parse("1.3.6.1.2.1.1.3.0");
    return sysUpTime;
}

const Oid& Oid::sysContact()
{
    static Oid sysContact = *parse("1.3.6.1.2.1.1.4.0");
    return sysContact;
}

const Oid& Oid::sysName()
{
    static Oid sysName = *parse("1.3.6.1.2.1.1.5.0");
    return sysName;
}

const Oid& Oid::sysLocation()
{
    static Oid sysLocation = *parse("1.3.6.1.2.1.1.6.0");
    return sysLocation;
}

std::string_view Oid::asDottedString() const
{
    return m_oid.get_printable();
}

std::string Oid::toString() const
{
    return m_oid.get_printable();
}

} // namespace net