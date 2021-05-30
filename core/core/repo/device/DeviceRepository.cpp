#include "DeviceRepository.h"
#include "core/models/net/Device.h"
#include "core/net/MacAddress.h"
#include "core/util/Result.h"

namespace repo
{

bool DeviceRepository::create(const net::MacAddress& mac, DevicePtr&& item)
{
    if (has(mac))
        return false;

    m_devices[mac] = std::move(item);
    return true;
}

Result<DeviceRepository::DevicePtr> DeviceRepository::read(const net::MacAddress& mac) const
{
    if (has(mac))
        return m_devices.at(mac);
    return Error(ErrorType::NotFound);
}

bool DeviceRepository::update(const net::MacAddress& mac, DevicePtr&& item)
{
    if (!has(mac))
        return false;

    m_devices[mac] = std::move(item);
    return true;
}

bool DeviceRepository::remove(const net::MacAddress& mac)
{
    if (!has(mac))
        return false;

    m_devices.erase(mac);
    return true;
}

bool DeviceRepository::has(const net::MacAddress& mac) const
{
    return m_devices.count(mac) != 0;
}

Result<DeviceRepository::DevicePtr> DeviceRepository::find(const net::IPv4Address& ip) const
{
    return findBy([&ip](const models::Device& device) { return device.ipAddress() == ip; });
}

Result<DeviceRepository::DevicePtr> DeviceRepository::find(const net::MacAddress& mac) const
{
    if (!has(mac))
        return Error(ErrorType::NotFound);
    return m_devices.at(mac);
}

} // namespace repo