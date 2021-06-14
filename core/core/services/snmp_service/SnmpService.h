#pragma once

#include "core/net/IPv4Address.h"
#include "core/net/snmp/Oid.h"
#include "core/net/snmp/SnmpClient.h"
#include "core/net/snmp/SnmpTarget.h"
#include "core/util/Result.h"
#include "core/util/async/FutureResolver.h"
#include "core/util/logger/Logger.h"
#include <optional>

namespace service
{
class SnmpService
{
    class SnmpResolver : public util::v2::FutureResolver<Result<net::SnmpResponse>>
    {
      public:
        void onCompletion(const Result<net::SnmpResponse>& item) override
        {
            if (item)
            {
                CORE_INFO("{}: {}", item->host, fmt::join(item->variables, ",\n\t"));
            }
            else
            {
                CORE_INFO("Failed request");
            }
        }
        void onException(const std::exception_ptr&) override
        {
            CORE_WARN("Error");
        }
    };

    net::SnmpClient& m_client;

  public:
    SnmpService(net::SnmpClient& client)
        : m_client(client)
    {
    }

    void requestSystem(const net::IPv4Address& host, std::optional<std::size_t> port = {})
    {
        const std::array sysOids = { net::Oid::sysName(), net::Oid::sysDescr(), net::Oid::sysUpTime() };
        const auto resolver = std::make_shared<SnmpResolver>();

        auto target = net::CommunityTarget::createV2(host);
        if (port)
            target.setPort(port.value());

        resolver->setFuture(m_client.walkAsync(net::Oid::ifTable(), target));
        util::TaskRunner::defaultRunner()
            .post(resolver);
    }
};
} // namespace service