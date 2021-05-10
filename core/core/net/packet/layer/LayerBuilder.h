#pragma once

#include <Packet.h>
#include <cstdint>
#include <vector>

namespace net
{
class LayerBuilder
{
  public:
    virtual void build(std::vector<std::uint8_t>& data) = 0;
    virtual void build(pcpp::Packet& data) = 0;

  protected:
    LayerBuilder() = default;
};
} // namespace net