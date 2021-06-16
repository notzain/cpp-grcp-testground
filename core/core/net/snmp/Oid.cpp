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

std::string Oid::toString() const
{
    return m_oid.get_printable();
}

} // namespace net