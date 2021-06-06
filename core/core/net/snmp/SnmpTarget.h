#pragma once

#include "SnmpClient.h"
#include "SnmpDefs.h"

#include "core/net/IPv4Address.h"
#include "core/util/Badge.h"
#include "core/util/Time.h"

#include <memory>
#include <snmp_pp/target.h>

namespace net
{
class SnmpTarget
{
    IPv4Address m_target;
    SnmpVersion m_version;
    Milliseconds m_timeout;
    std::size_t m_retries;

  public:
    SnmpTarget(SnmpVersion version, IPv4Address target)
        : m_target(std::move(target))
        , m_version(version)
        , m_timeout(Seconds(5))
        , m_retries(3)
    {
    }

    const IPv4Address& target() const { return m_target; }
    void setTarget(IPv4Address address) { m_target = std::move(address); }

    SnmpVersion version() const { return m_version; }
    void setVersion(SnmpVersion version) { m_version = version; }

    const Milliseconds& timeout() const { return m_timeout; }
    void setTimeout(Milliseconds ms) { m_timeout = std::move(ms); }

    std::size_t retries() const { return m_retries; }
    void setRetries(std::size_t retries) { m_retries = retries; }

    virtual std::unique_ptr<Snmp_pp::SnmpTarget> toTarget(Badge<SnmpClient>) const = 0;
};

class CommunityTarget : public SnmpTarget
{
    std::string m_readCommunity;
    std::string m_writeCommunity;

  public:
    static CommunityTarget createV1(IPv4Address target, std::string_view readCommunity = "public", std::string_view writeCommunity = "public")
    {
        return CommunityTarget(SnmpVersion::V1, std::move(target), readCommunity, writeCommunity);
    }

    static CommunityTarget createV2(IPv4Address target, std::string_view readCommunity = "public", std::string_view writeCommunity = "public")
    {
        return CommunityTarget(SnmpVersion::V2, std::move(target), readCommunity, writeCommunity);
    }

    std::string_view readCommunity() const { return m_readCommunity; }
    void setReadCommunity(std::string community) { m_readCommunity = std::move(community); }

    std::string_view writeCommunity() const { return m_writeCommunity; }
    void setWriteCommunity(std::string community) { m_writeCommunity = std::move(community); }

    std::unique_ptr<Snmp_pp::SnmpTarget> toTarget(Badge<SnmpClient>) const override
    {
        auto snmpTarget = std::make_unique<Snmp_pp::CTarget>(Snmp_pp::IpAddress(target().asString().data()), m_readCommunity.data(), m_writeCommunity.data());
        snmpTarget->set_retry(retries());
        snmpTarget->set_timeout(timeout().count());
        snmpTarget->set_version((Snmp_pp::snmp_version)version());

        return snmpTarget;
    }

  private:
    CommunityTarget(SnmpVersion version, IPv4Address target, std::string_view readCommunity, std::string_view writeCommunity)
        : SnmpTarget(version, std::move(target))
        , m_readCommunity(readCommunity.data())
        , m_writeCommunity(writeCommunity.data())
    {
    }
};

class UserTarget : public SnmpTarget
{
};
} // namespace net