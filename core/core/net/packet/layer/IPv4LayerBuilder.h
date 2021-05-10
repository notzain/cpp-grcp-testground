#pragma once

#include "LayerBuilder.h"

#include <cstdint>
#include <memory>
#include <string>

namespace net
{
class IPv4LayerBuilder : public LayerBuilder
{
    std::string m_srcIp;
    std::string m_dstIp;
    std::uint16_t m_id{ 0 };
    std::uint8_t m_ttl{ 64 };

  public:
    static IPv4LayerBuilder create() { return IPv4LayerBuilder(); }

    IPv4LayerBuilder& withSrcIp(std::string_view ip);
    IPv4LayerBuilder& withDstIp(std::string_view ip);
    IPv4LayerBuilder& withId(std::uint16_t id);
    IPv4LayerBuilder& withTimeToLive(std::uint8_t ttl);

    void build(std::vector<std::uint8_t>& data) override;
    void build(pcpp::Packet& data) override;
};
} // namespace net