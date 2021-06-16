#include "SnmpClient.h"

#include "SnmpTarget.h"
#include "core/util/Defer.h"
#include "core/util/logger/Logger.h"

#include <cstdint>
#include <snmp_pp/address.h>
#include <snmp_pp/config_snmp_pp.h>
#include <snmp_pp/oid.h>
#include <snmp_pp/pdu.h>
#include <snmp_pp/smi.h>
#include <snmp_pp/snmperrs.h>
#include <snmp_pp/vb.h>
namespace net
{
static Snmp_pp::Pdu createPdu(nonstd::span<const Oid> oids);
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

    auto v3Model = initializeV3();
    if (!v3Model)
    {
        return Error(ErrorType::Unknown);
    }

    return std::shared_ptr<SnmpClient>(new SnmpClient(std::move(snmp), std::move(*v3Model)));
}

SnmpClient::SnmpClient(std::unique_ptr<Snmp_pp::Snmp> snmp, std::unique_ptr<Snmp_pp::v3MP> v3)
    : m_snmp(std::move(snmp))
    , m_v3Model(std::move(v3))
{
    m_snmp->set_mpv3(m_v3Model.get());
    m_snmp->start_poll_thread(10);
}

SnmpClient::~SnmpClient() { m_snmp->stop_poll_thread(); }

Result<SnmpResponse> SnmpClient::get(nonstd::span<const Oid> oids, const SnmpTarget& target)
{
    return getAsync(oids, target).get();
}

std::future<Result<SnmpResponse>> SnmpClient::getAsync(nonstd::span<const Oid> oids, const SnmpTarget& target)
{
    auto pdu = createPdu(oids);
    auto& request = createRequest(target);

    auto snmpTarget = target.toTarget({});
    m_snmp->get(pdu, *snmpTarget, callback, &request);
    return request.promise.get_future();
}

Result<SnmpResponse> SnmpClient::getBulk(nonstd::span<const Oid> oids, const SnmpTarget& target, int nonRepeaters, int maxReps)
{
    return getBulkAsync(oids, target, nonRepeaters, maxReps).get();
}

std::future<Result<SnmpResponse>> SnmpClient::getBulkAsync(nonstd::span<const Oid> oids, const SnmpTarget& target, int nonRepeaters, int maxReps)
{
    auto pdu = createPdu(oids);
    auto& request = createRequest(target);

    auto snmpTarget = target.toTarget({});
    m_snmp->get_bulk(pdu, *snmpTarget, nonRepeaters, maxReps, callback, &request);
    return request.promise.get_future();
}

Result<SnmpResponse> SnmpClient::walk(const Oid& oid, const SnmpTarget& target)
{
    return walkAsync(oid, target).get();
}

std::future<Result<SnmpResponse>> SnmpClient::walkAsync(const Oid& oid, const SnmpTarget& target)
{
    auto pdu = createPdu({ &oid, 1 });
    auto& request = createRequest(target);
    request.isWalk = true;
    request.walkOid = oid;

    auto snmpTarget = target.toTarget({});
    m_snmp->get_bulk(pdu, *snmpTarget, 0, 10, callback, &request);
    return request.promise.get_future();
}

void SnmpClient::addUserAuthentication(const SnmpUserAuthentication& user)
{
    m_v3Model->get_usm()->add_usm_user(user.username.data(),
                                       static_cast<int>(user.authProtocol),
                                       static_cast<int>(user.privProtocol),
                                       user.authPassword.data(),
                                       user.privPassword.data());
}

void SnmpClient::removeUserAuthentication(const std::string& user)
{
    m_v3Model->get_usm()->delete_usm_user(user.data());
}

Result<std::unique_ptr<Snmp_pp::v3MP>> SnmpClient::initializeV3()
{
    const char* engineId = "snmp_engine_proc";
    const char* filename = "snmpv3_boot_counter";
    unsigned int snmpEngineBoots = 0;
    int status;

    status = Snmp_pp::getBootCounter(filename, engineId, snmpEngineBoots);
    if ((status != SNMPv3_OK) && (status < SNMPv3_FILEOPEN_ERROR))
    {
        CORE_ERROR("Error loading snmpEngineBoots counter: {}", status);
        return Error(ErrorType::Unavailable);
    }
    snmpEngineBoots++;
    status = Snmp_pp::saveBootCounter(filename, engineId, snmpEngineBoots);
    if (status != SNMPv3_OK)
    {
        CORE_ERROR("Error saving snmpEngineBoots counter: {}", status);
        return Error(ErrorType::Unavailable);
    }

    auto v3_MP = std::make_unique<Snmp_pp::v3MP>(engineId, snmpEngineBoots, status);
    if (status != SNMPv3_MP_OK)
    {
        CORE_ERROR("Error initializing v3MP: {}", status);
        return Error(ErrorType::Unavailable);
    }
    return v3_MP;
}

void SnmpClient::callback(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, void* requestPtr)
{
    auto* request = static_cast<SnmpRequest*>(requestPtr);

    if (request->isWalk)
    {
        handleWalk(reason, snmp, pdu, target, request);
    }
    else
    {
        handleGet(reason, snmp, pdu, target, request);
    }
}

void SnmpClient::handleGet(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, SnmpRequest* request)
{
    defer(request->self->m_pendingRequests.erase(request->uuid));
    if (reason == SNMP_CLASS_ASYNC_RESPONSE)
    {
        Snmp_pp::Vb nextVb;

        for (int i = 0; i < pdu.get_vb_count(); i++)
        {
            request->response.host = request->host;
            pdu.get_vb(nextVb, i);
            if (nextVb.valid())
                request->response.variables.push_back({ *Oid::parse(nextVb.get_printable_oid()),
                                                        toOidValue(nextVb) });
        }
        request->promise.set_value(request->response);
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

void SnmpClient::handleWalk(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, SnmpRequest* request)
{
    if (reason == SNMP_CLASS_TIMEOUT)
    {
        request->promise.set_value(Error(ErrorType::TimedOut));
        return;
    }

    Snmp_pp::Oid requestOid(request->walkOid->toString().data());

    Snmp_pp::Vb nextVb;
    for (int i = 0; i < pdu.get_vb_count(); i++)
    {
        pdu.get_vb(nextVb, i);

        Snmp_pp::Oid currentOid;
        nextVb.get_oid(currentOid);

        if (requestOid.nCompare(requestOid.len(), currentOid) != 0)
        {
            request->promise.set_value(request->response);
            return;
        }

        if (nextVb.get_syntax() != sNMP_SYNTAX_ENDOFMIBVIEW)
        {
            request->response.variables.push_back({ *Oid::parse(nextVb.get_printable_oid()),
                                                    toOidValue(nextVb) });
        }
        else
        {
            request->promise.set_value(Error(ErrorType::TimedOut));
            return;
        }
    }
    pdu.set_vblist(&nextVb, 1);
    snmp->get_bulk(pdu, target, 0, 10, callback, request);
}

SnmpClient::SnmpRequest& SnmpClient::createRequest(const SnmpTarget& target)
{
    const auto uuid = Uuid::generate();
    m_pendingRequests[uuid] = SnmpRequest{ uuid, target.ipAddress(), {}, this };
    return m_pendingRequests[uuid];
}

Snmp_pp::Pdu createPdu(nonstd::span<const Oid> oids)
{
    Snmp_pp::Pdu pdu;
    for (const auto& oid : oids)
    {
        Snmp_pp::Vb vb;
        vb.set_oid(oid.toString().data());
        pdu += vb;
    }
    return pdu;
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
    case sNMP_SYNTAX_OID: {
        Snmp_pp::Oid oid;
        vb.get_value(oid);
        return Oid::OidType(*Oid::parse(oid.get_printable()));
    };
    default: {
        CORE_WARN("Could not convert OID '{}' to value: '{}'", vb.get_printable_oid(), vb.get_printable_value());
    }
    }

    return Oid::Opaque(vb.get_printable_value());
}

} // namespace net