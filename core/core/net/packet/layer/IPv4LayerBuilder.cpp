#include "IPv4LayerBuilder.h"

#include <IPv4Layer.h>
#include <IpAddress.h>

namespace net
{

IPv4LayerBuilder& IPv4LayerBuilder::withSrcIp(std::string_view ip)
{
    m_srcIp = ip;
    return *this;
}
IPv4LayerBuilder& IPv4LayerBuilder::withDstIp(std::string_view ip)
{
    m_dstIp = ip;
    return *this;
}
IPv4LayerBuilder& IPv4LayerBuilder::withId(std::uint16_t id)
{
    m_id = id;
    return *this;
}
IPv4LayerBuilder& IPv4LayerBuilder::withTimeToLive(std::uint8_t ttl)
{
    m_ttl = ttl;
    return *this;
}

void IPv4LayerBuilder::build(std::vector<std::uint8_t>& data)
{
    pcpp::IPv4Layer ip({ m_srcIp }, { m_dstIp });
    ip.getIPv4Header()->ipId = m_id;
    ip.getIPv4Header()->timeToLive = m_ttl;
    ip.computeCalculateFields();

    for (int i = 0; i < ip.getDataLen(); ++i)
        data.push_back(ip.getData()[i]);
    for (int i = 0; i < ip.getLayerPayloadSize(); ++i)
        data.push_back(ip.getLayerPayload()[i]);
}
void IPv4LayerBuilder::build(pcpp::Packet& data)
{
    auto ip = new pcpp::IPv4Layer({ m_srcIp }, { m_dstIp });
    ip->getIPv4Header()->ipId = m_id;
    ip->getIPv4Header()->timeToLive = m_ttl;
    data.addLayer(ip, true);
}

} // namespace net