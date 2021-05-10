#pragma once

#include "LayerBuilder.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

namespace net
{
class IcmpLayerBuilder : public LayerBuilder
{
    std::uint16_t m_id;
    std::uint16_t m_sequence{ 0 };
    std::uint64_t m_timestamp{ 0 };
    std::string m_payload;

  public:
    static IcmpLayerBuilder create() { return IcmpLayerBuilder(); }

    IcmpLayerBuilder& withId(std::uint16_t id);
    IcmpLayerBuilder& withSequence(std::uint16_t sequence);
    IcmpLayerBuilder& withTimestamp(std::uint64_t timestamp =
                                        std::chrono::duration_cast<std::chrono::milliseconds>(
                                            std::chrono::system_clock::now().time_since_epoch())
                                            .count());
    IcmpLayerBuilder& withPayload(std::string payload);

    void build(std::vector<std::uint8_t>& data) override;
    void build(pcpp::Packet& data) override;
};
} // namespace net