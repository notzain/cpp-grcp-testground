#include "EthLayerBuilder.h"
#include <EthLayer.h>

namespace net
{
EthLayerBuilder& EthLayerBuilder::withSrcMac(std::string_view mac)
{
    m_srcMac = mac;
    return *this;
}
EthLayerBuilder& EthLayerBuilder::withDstMac(std::string_view mac)
{
    m_dstMac = mac;
    return *this;
}

void EthLayerBuilder::build(std::vector<std::uint8_t>& data)
{
    pcpp::EthLayer eth(m_srcMac, m_dstMac);
    eth.computeCalculateFields();
    for (int i = 0; i < eth.getDataLen(); ++i)
        data.push_back(eth.getData()[i]);
    for (int i = 0; i < eth.getLayerPayloadSize(); ++i)
        data.push_back(eth.getLayerPayload()[i]);
}

void EthLayerBuilder::build(pcpp::Packet& data)
{
    auto* eth = new pcpp::EthLayer(m_srcMac, m_dstMac);
    data.addLayer(eth, true);
}
} // namespace net