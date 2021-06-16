#pragma once

#include "SnmpTarget.h"

#include "core/net/IPv4Address.h"
#include "core/net/snmp/SnmpTarget.h"

namespace net
{
class ISnmpTargetFactory
{
  public:
    virtual std::unique_ptr<SnmpTarget> createTarget(const IPv4Address& host) const = 0;
    virtual std::unique_ptr<SnmpTarget> createTarget(const IPv4Address& host, std::size_t port) const = 0;
};
} // namespace net