#pragma once

#include "Oid.h"
#include "VarBind.h"
#include "core/net/IPv4Address.h"
#include "core/util/Result.h"

#include <future>
#include <memory>
#include <nonstd/span.hpp>
#include <snmp_pp/snmp_pp.h>

namespace net
{
class SnmpTarget;

class SnmpResponse
{
    IPv4Address host;
    std::vector<VarBind> variables;
};

class SnmpClient
{
    std::unique_ptr<Snmp_pp::Snmp> m_snmp;

  public:
    using Ptr = std::shared_ptr<SnmpClient>;

    static Result<SnmpClient::Ptr> create(std::size_t port = 0);

    ~SnmpClient();

    Result<SnmpResponse> get(const Oid& oid, const SnmpTarget& target) { return get({ &oid, 1 }, target); };
    Result<SnmpResponse> get(nonstd::span<const Oid> oids, const SnmpTarget& target);

    std::future<Result<SnmpResponse>> getAsync(const Oid& oid, const SnmpTarget& target) { return getAsync({ &oid, 1 }, target); };
    std::future<Result<SnmpResponse>> getAsync(nonstd::span<const Oid> oids, const SnmpTarget& target);

  private:
    SnmpClient(std::unique_ptr<Snmp_pp::Snmp> snmp);

    static void callback(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, void* cd);
};
} // namespace net