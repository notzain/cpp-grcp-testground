#pragma once

#include "core/util/Enum.h"

namespace net
{
enum class SnmpVersion
{
    V1,
    V2,
    UnusedV2,
    V3
};

enum class SnmpSecurity
{
    NoAuthNoPriv = 1,
    AuthNoPriv,
    AuthPriv
};

} // namespace net

REGISTER_ENUM(net::SnmpVersion);
REGISTER_ENUM(net::SnmpSecurity);