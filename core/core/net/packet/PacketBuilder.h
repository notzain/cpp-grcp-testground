#pragma once

#include <cstdint>
#include <memory>
#include <nonstd/span.hpp>
#include <vector>

#include <Packet.h>

namespace net
{
class PacketBuilder
{
    pcpp::Packet m_packet;

  public:
    template <typename LayerBuilder>
    PacketBuilder& addLayer(LayerBuilder& layerBuilder)
    {
        layerBuilder.build(m_packet);
        m_packet.computeCalculateFields();
        return *this;
    }

    nonstd::span<std::uint8_t> getData() const { return { (std::uint8_t*)m_packet.getRawPacket()->getRawData(), (size_t)m_packet.getRawPacket()->getRawDataLen() }; }
};

}; // namespace net