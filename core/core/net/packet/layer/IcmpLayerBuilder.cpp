#include "IcmpLayerBuilder.h"

#include <IcmpLayer.h>
#include <chrono>
#include <cstdint>

namespace net
{
IcmpLayerBuilder& IcmpLayerBuilder::withId(std::uint16_t id)
{
    m_id = id;
    return *this;
}

IcmpLayerBuilder& IcmpLayerBuilder::withSequence(std::uint16_t sequence)
{
    m_sequence = sequence;
    return *this;
}

IcmpLayerBuilder& IcmpLayerBuilder::withTimestamp(std::uint64_t timestamp)
{
    m_timestamp = timestamp;
    return *this;
}

IcmpLayerBuilder& IcmpLayerBuilder::withPayload(std::string payload)
{
    m_payload = std::move(payload);
    return *this;
}

void IcmpLayerBuilder::build(std::vector<std::uint8_t>& data)
{
    pcpp::IcmpLayer icmp;
    icmp.setEchoRequestData(m_id, m_sequence, m_timestamp, (const std::uint8_t*)m_payload.data(), m_payload.size());
    icmp.computeCalculateFields();

    for (int i = 0; i < icmp.getDataLen(); ++i)
        data.push_back(icmp.getData()[i]);
    for (int i = 0; i < icmp.getLayerPayloadSize(); ++i)
        data.push_back(icmp.getLayerPayload()[i]);
}

void IcmpLayerBuilder::build(pcpp::Packet& data)
{
    auto icmp = new pcpp::IcmpLayer;
    icmp->setEchoRequestData(m_id, m_sequence, m_timestamp, (const std::uint8_t*)m_payload.data(), m_payload.size());
    data.addLayer(icmp, true);
}

} // namespace net