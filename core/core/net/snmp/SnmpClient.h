#pragma once

#include "Oid.h"
#include "SnmpDefs.h"
#include "VarBind.h"
#include "core/net/IPv4Address.h"
#include "core/util/Result.h"
#include "core/util/Uuid.h"

#include <future>
#include <memory>
#include <nonstd/span.hpp>
#include <snmp_pp/mp_v3.h>
#include <snmp_pp/snmp_pp.h>

namespace net
{
class SnmpTarget;

struct SnmpResponse
{
    IPv4Address host;
    std::vector<VarBind> variables;
};

struct SnmpUserAuthentication
{
    std::string username;
    SnmpAuthProtocol authProtocol;
    SnmpPrivProtocol privProtocol;
    std::string authPassword;
    std::string privPassword;
};

class SnmpClient
{
    std::unique_ptr<Snmp_pp::Snmp> m_snmp;
    std::unique_ptr<Snmp_pp::v3MP> m_v3Model;

    struct SnmpRequest
    {
        Uuid uuid = Uuid::nil();
        IPv4Address host;
        std::promise<Result<SnmpResponse>> promise;
        SnmpClient* self;
        SnmpResponse response;

        bool isWalk = false;
        std::optional<Oid> walkOid;
    };
    std::map<Uuid, SnmpRequest> m_pendingRequests;

  public:
    using Ptr = std::shared_ptr<SnmpClient>;

    static Result<SnmpClient::Ptr> create(std::size_t port = 0);

    ~SnmpClient();

    Result<SnmpResponse> get(const Oid& oid, const SnmpTarget& target) { return get({ &oid, 1 }, target); };
    Result<SnmpResponse> get(nonstd::span<const Oid> oids, const SnmpTarget& target);
    std::future<Result<SnmpResponse>> getAsync(const Oid& oid, const SnmpTarget& target) { return getAsync({ &oid, 1 }, target); };
    std::future<Result<SnmpResponse>> getAsync(nonstd::span<const Oid> oids, const SnmpTarget& target);

    Result<SnmpResponse> getBulk(const Oid& oid, const SnmpTarget& target, int nonRepeaters, int maxReps) { return getBulk({ &oid, 1 }, target, nonRepeaters, maxReps); };
    Result<SnmpResponse> getBulk(nonstd::span<const Oid> oids, const SnmpTarget& target, int nonRepeaters, int maxReps);
    std::future<Result<SnmpResponse>> getBulkAsync(const Oid& oid, const SnmpTarget& target, int nonRepeaters, int maxReps) { return getBulkAsync({ &oid, 1 }, target, nonRepeaters, maxReps); };
    std::future<Result<SnmpResponse>> getBulkAsync(nonstd::span<const Oid> oids, const SnmpTarget& target, int nonRepeaters, int maxReps);

    Result<SnmpResponse> walk(const Oid& oid, const SnmpTarget& target);
    std::future<Result<SnmpResponse>> walkAsync(const Oid& oid, const SnmpTarget& target);

    void addUserAuthentication(const SnmpUserAuthentication& user);
    void removeUserAuthentication(const std::string& user);

  private:
    SnmpClient(std::unique_ptr<Snmp_pp::Snmp> snmp, std::unique_ptr<Snmp_pp::v3MP> v3);

    SnmpRequest& createRequest(const SnmpTarget& target);

    static Result<std::unique_ptr<Snmp_pp::v3MP>> initializeV3();
    static void callback(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, void* cd);
    static void handleGet(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, SnmpRequest* request);
    static void handleWalk(int reason, Snmp_pp::Snmp* snmp, Snmp_pp::Pdu& pdu, Snmp_pp::SnmpTarget& target, SnmpRequest* request);
};
} // namespace net