#include "SnmpClient.h"

#include "SnmpTarget.h"
#include "core/util/Defer.h"
#include "core/util/logger/Logger.h"

#include <cstdint>
#include <snmp_pp/address.h>
#include <snmp_pp/config_snmp_pp.h>
#include <snmp_pp/pdu.h>
#include <snmp_pp/smi.h>
#include <snmp_pp/snmperrs.h>
#include <snmp_pp/vb.h>
namespace net
{
static OidValue toOidValue(const Snmp_pp::Vb& vb);

Result<SnmpClient::Ptr> SnmpClient::create(std::size_t port)
{
    Snmp_pp::DefaultLog::log()->set_profile("off");
    int status = 0;
    auto snmp = std::make_unique<Snmp_pp::Snmp>(status, port, false);
    if (status)
    {
        return Error(ErrorType::Unknown);
    }

    return std::shared_ptr<SnmpClient>(new SnmpClient(std::move(snmp)));
}

SnmpClient::SnmpClient(std::unique_ptr<Snmp_pp::Snmp> snmp)
    : m_snmp(std::move(snmp))
{
    m_snmp->start_poll_thread(10);
}

SnmpClient::~SnmpClient() { m_snmp->stop_poll_thread(); }

Result<SnmpResponse> SnmpClient::get(nonstd::span<const Oid> oids, const SnmpTarget& target)
{
    return getAsync(oids, target).get();
}

std::future<Result<SnmpResponse>> SnmpClient::getAsync(nonstd::span<const Oid> oids, const SnmpTarget& target)
{
    Snmp_pp::Pdu pdu;
    for (const auto& oid : oids)
    {
        Snmp_pp::Vb vb;
        vb.set_oid(oid.asDottedString().data());
        pdu += vb;
    }

    const auto uuid = Uuid::generate();
    m_pendingRequests[uuid] = SnmpRequest{ uuid, target.target(), {}, this };
    auto& request = m_pendingRequests[uuid];

    auto snmpTarget = target.toTarget({});
    m_snmp->get(pdu, *snmpTarget, callback, &request);
    return request.promise.get_future();
}

void SnmpClient::callback(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, void* requestPtr)
{
    auto* request = static_cast<SnmpRequest*>(requestPtr);
    defer(request->self->m_pendingRequests.erase(request->uuid));

    if (reason == SNMP_CLASS_ASYNC_RESPONSE)
    {
        SnmpResponse response;
        Snmp_pp::Vb nextVb;
        response.host = request->host;

        for (int i = 0; i < pdu.get_vb_count(); i++)
        {
            pdu.get_vb(nextVb, i);
            response.variables.push_back({ *Oid::parse(nextVb.get_printable_oid()), toOidValue(nextVb) });
        }
        request->promise.set_value(response);
    }
    else if (reason == SNMP_CLASS_TIMEOUT)
    {
        request->promise.set_value(Error(ErrorType::TimedOut));
    }
    else
    {
        request->promise.set_value(Error(ErrorType::Unknown));
    }
}

OidValue toOidValue(const Snmp_pp::Vb& vb)
{
    switch (vb.get_syntax())
    {
    case sNMP_SYNTAX_INT: {
        int i;
        vb.get_value(i);
        return Oid::Integer(i);
    }
    case sNMP_SYNTAX_OCTETS: {
        char octet[256] = { 0 };
        unsigned long len = 0;
        vb.get_value((unsigned char*)octet, len, 256, true);
        return Oid::OctetString(std::string(octet));
    }
    case sNMP_SYNTAX_CNTR32: {
        std::uint32_t i;
        vb.get_value(i);
        return Oid::Counter32(i);
    }
    case sNMP_SYNTAX_GAUGE32: {
        std::uint32_t i;
        vb.get_value(i);
        return Oid::Gauge32(i);
    }
    case sNMP_SYNTAX_TIMETICKS: {
        std::uint32_t i;
        vb.get_value(i);
        return Oid::TimeTicks(i);
    }
    case sNMP_SYNTAX_CNTR64: {
        pp_uint64 i;
        vb.get_value(i);
        return Oid::Counter64(i);
    }
    default: {
        CORE_WARN("Could not convert OID '{}' to value: '{}'", vb.get_printable_oid(), vb.get_printable_value());
    }
    }

    return Oid::Opaque("");
}

} // namespace net