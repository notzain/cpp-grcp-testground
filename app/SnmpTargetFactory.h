#pragma once

#include "core/models/net/Device.h"
#include "core/net/snmp/SnmpTarget.h"
#include <core/net/IPv4Address.h>
#include <core/net/MacAddress.h>
#include <core/net/snmp/SnmpDefs.h>
#include <core/net/snmp/SnmpTargetFactory.h>
#include <core/repo/device/DeviceRepository.h>

class SnmpTargetFactory : public net::ISnmpTargetFactory
{
    repo::DeviceRepository& m_deviceRepo;

  public:
    SnmpTargetFactory(repo::DeviceRepository& repo)
        : m_deviceRepo(repo)
    {
    }

    std::unique_ptr<net::SnmpTarget> createTarget(const net::IPv4Address& host) const override
    {
        auto deviceOrError = m_deviceRepo.find(host);
        if (deviceOrError)
        {
            const auto& device = *deviceOrError;
            auto version = device->lastUsedSnmpVersion().value_or(net::SnmpVersion::V2);
            switch (version)
            {
            case net::SnmpVersion::V1:
                return createTargetV1(*device);
            case net::SnmpVersion::V2:
                return createTargetV2(*device);
            case net::SnmpVersion::V3:
                return createTargetV3(*device);
            default:
                return createTargetV2(*device);
            }
        }
        else
        {
            return std::make_unique<net::CommunityTarget>(net::CommunityTarget::createV2(host));
        }
    }

    std::unique_ptr<net::SnmpTarget> createTarget(const net::IPv4Address& host, std::size_t port) const override
    {
        auto target = createTarget(host);
        target->setPort(port);
        return target;
    }

  private:
    std::unique_ptr<net::SnmpTarget> createTargetV1(const models::Device& device) const
    {
        return std::make_unique<net::CommunityTarget>(net::CommunityTarget::createV1(device.ipAddress()));
    }

    std::unique_ptr<net::SnmpTarget> createTargetV2(const models::Device& device) const
    {
        return std::make_unique<net::CommunityTarget>(net::CommunityTarget::createV2(device.ipAddress()));
    }

    std::unique_ptr<net::SnmpTarget> createTargetV3(const models::Device& device) const
    {
        return nullptr;
        // return std::make_unique<net::UserTarget>(net::CommunityTarget::createV2(device.ipAddress()));
    }
};