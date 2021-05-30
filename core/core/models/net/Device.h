#pragma once

#include "core/net/IPv4Address.h"
#include "core/net/MacAddress.h"
#include "core/traits/Printable.h"
#include "core/traits/serializer/JsonSerializer.h"

namespace models
{
class Device
    : public traits::Printable<Device>
    , public traits::JsonSerializer
{
    net::IPv4Address m_ipAddress;
    net::MacAddress m_macAddress;

  public:
    Device(net::IPv4Address ip, net::MacAddress mac);
    virtual ~Device() = default;

    net::IPv4Address ipAddress() const;
    net::MacAddress macAddress() const;

    std::string format() const override;
    nlohmann::json serialize(nlohmann::json json = {}) const override;
};
} // namespace models