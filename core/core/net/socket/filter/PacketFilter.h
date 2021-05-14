#pragma once

#include <type_traits>
struct sock_fprog;

namespace net
{
class PacketFilter
{
  public:
    bool attach(int socketFd);
    bool detach(int socketFd);

    template <typename Filter>
    static PacketFilter& get()
    {
        static_assert(std::is_base_of_v<PacketFilter, Filter>, "Filter must derive from PacketFilter.");
        static Filter filter;
        return filter;
    }

  private:
    // generate this from tcp-dump
    // example: sudo tcpdump icmp -dd
    virtual sock_fprog* getPacketFilter() const = 0;
};

} // namespace net