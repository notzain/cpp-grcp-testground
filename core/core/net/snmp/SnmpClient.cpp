#include "SnmpClient.h"

#include "SnmpTarget.h"
#include "core/util/logger/Logger.h"

#include <snmp_pp/address.h>
#include <snmp_pp/pdu.h>

namespace net
{
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
    Snmp_pp::Pdu pdu;
    for (const auto& oid : oids)
    {
        Snmp_pp::Vb vb;
        vb.set_oid(oid.asDottedString().data());
        pdu += vb;
    }

    auto snmpTarget = target.toTarget({});
    int status = m_snmp->get(pdu, *snmpTarget);

    for (int i = 0; i < pdu.get_vb_count(); i++)
    {
        Snmp_pp::Vb vb;
        pdu.get_vb(vb, i);
        CORE_INFO("{} : {}", vb.get_printable_oid(), vb.get_printable_value());
    }
    return {};
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

    auto snmpTarget = target.toTarget({});
    int status = m_snmp->get(pdu, *snmpTarget);

    m_snmp->get(pdu, *snmpTarget, callback, nullptr);
    return {};
}

void SnmpClient::callback(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, void* cd)
{
    Snmp_pp::Vb nextVb;
    int pdu_error;

    std::cout << "reason: " << reason << std::endl
              << "msg: " << snmp->error_msg(reason) << std::endl;

    pdu_error = pdu.get_error_status();
    if (pdu_error)
    {
        std::cout << "Response contains error: "
                  << snmp->error_msg(pdu_error) << std::endl;
    }
    for (int i = 0; i < pdu.get_vb_count(); i++)
    {
        pdu.get_vb(nextVb, i);

        std::cout << "Oid: " << nextVb.get_printable_oid() << std::endl
                  << "Val: " << nextVb.get_printable_value() << std::endl;
    }

    if (pdu.get_type() == sNMP_PDU_INFORM)
    {
        std::cout << "pdu type: " << pdu.get_type() << std::endl;
        std::cout << "sending response to inform: " << std::endl;
        nextVb.set_value("This is the response.");
        pdu.set_vb(nextVb, 0);
        snmp->response(pdu, target);
    }
    std::cout << std::endl;
}

} // namespace net