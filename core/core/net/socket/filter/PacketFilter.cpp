#include "PacketFilter.h"

#include <cassert>
#include <linux/filter.h>
#include <sys/socket.h>

namespace net
{
bool PacketFilter::attach(int socketFd)
{
    auto* bpf = getPacketFilter();
    assert(bpf);
    return setsockopt(socketFd, SOL_SOCKET, SO_ATTACH_FILTER, bpf, sizeof(*bpf)) > 0;
}

bool PacketFilter::detach(int socketFd)
{
    auto* bpf = getPacketFilter();
    assert(bpf);
    return setsockopt(socketFd, SOL_SOCKET, SO_DETACH_FILTER, bpf, sizeof(*bpf)) > 0;
}
} // namespace net