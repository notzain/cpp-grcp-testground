#pragma once

#include "LayerBuilder.h"

#include <memory>
#include <string>

namespace net
{
class EthLayerBuilder : public LayerBuilder
{
    std::string m_srcMac;
    std::string m_dstMac;

  public:
    static EthLayerBuilder create() { return EthLayerBuilder(); }

    EthLayerBuilder& withSrcMac(std::string_view mac);
    EthLayerBuilder& withDstMac(std::string_view mac);

    void build(std::vector<std::uint8_t>& data) override;
    void build(pcpp::Packet& data) override;
};
} // namespace net