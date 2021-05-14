#pragma once

#include <Packet.h>
#include <bits/types/struct_timespec.h>
#include <cstdint>
#include <nonstd/span.hpp>

namespace net
{
class PacketParser
{
  public:
    PacketParser(nonstd::span<std::uint8_t> data)
        : m_rawPacket(data.data(), data.size(), timespec{}, false)
        , m_parsedPacket(&m_rawPacket)
    {
    }

    const pcpp::Packet& getPacket() const { return m_parsedPacket; }

  private:
    pcpp::RawPacket m_rawPacket;
    pcpp::Packet m_parsedPacket;
};
} // namespace net