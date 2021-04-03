#pragma once

#include <IpAddress.h>
#include <MacAddress.h>

#include "core/traits/Printable.h"
#include "core/traits/serializer/JsonSerializer.h"

namespace models
{
class Device
    : public traits::Printable<Device>
    , public traits::JsonSerializer
{
    pcpp::IPv4Address m_ipAddress;
    pcpp::MacAddress m_macAddress;

  public:
    Device(std::string_view ip, std::string_view mac);
    virtual ~Device() = default;

    pcpp::IPv4Address ipAddress() const;
    pcpp::MacAddress macAddress() const;

    std::string format() const override;
    nlohmann::json serialize(nlohmann::json json = {}) const override;
};
} // namespace models