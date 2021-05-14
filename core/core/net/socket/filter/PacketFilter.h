#pragma once

struct sock_fprog;

namespace net
{
class PacketFilter
{
  public:
    PacketFilter(sock_fprog* filter)
        : m_packetFilter(filter)
    {
    }

    bool attach(int socketFd);
    bool detach(int socketFd);

  private:
    // generate this from tcp-dump
    // example: sudo tcpdump icmp -dd
    sock_fprog* m_packetFilter;
};

} // namespace net