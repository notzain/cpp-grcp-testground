#pragma once

#include "PacketFilter.h"

namespace net
{
class IcmpFilter : public PacketFilter
{
  public:
    IcmpFilter() = default;

  private:
    sock_fprog* getPacketFilter() const override;
};
} // namespace net