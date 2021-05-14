#include "PacketFilter.h"

#include <cassert>
#include <linux/filter.h>
#include <sys/socket.h>

namespace net
{
bool PacketFilter::attach(int socketFd)
{
    assert(m_packetFilter);
    return setsockopt(socketFd, SOL_SOCKET, SO_ATTACH_FILTER, m_packetFilter, sizeof(*m_packetFilter)) > 0;
}

bool PacketFilter::detach(int socketFd)
{
    assert(m_packetFilter);
    return setsockopt(socketFd, SOL_SOCKET, SO_DETACH_FILTER, m_packetFilter, sizeof(*m_packetFilter)) > 0;
}
} // namespace net