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

enum class SnmpAuthProtocol
{
    None = 1,
    HMACMD5,
    HMACSHA,
    HMAC128SHA224,
    HMAC192SHA256,
    HMAC256SHA384,
    HMAC384SHA512,
};

enum class SnmpPrivProtocol
{
    None = 1,
    DES,
    DES3,
    AES128,
    IDEA = 9,
    AES192 = 20,
    AES256,
    AES128_3DES,
    AES192_3DES,
    AES256_3DES,
};

} // namespace net

REGISTER_ENUM(net::SnmpVersion);
REGISTER_ENUM(net::SnmpSecurity);
REGISTER_ENUM(net::SnmpAuthProtocol);
REGISTER_ENUM(net::SnmpPrivProtocol);