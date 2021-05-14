#include "IcmpFilter.h"

#include <array>
#include <linux/filter.h>

namespace net
{
namespace
{
std::array code = {
    sock_filter{ 0x28, 0, 0, 0x0000000c },
    sock_filter{ 0x15, 0, 3, 0x00000800 },
    sock_filter{ 0x30, 0, 0, 0x00000017 },
    sock_filter{ 0x15, 0, 1, 0x00000001 },
    sock_filter{ 0x6, 0, 0, 0x00040000 },
    sock_filter{ 0x6, 0, 0, 0x00000000 },
};
sock_fprog bpf = {
    .len = code.size(),
    .filter = code.data()
};
} // namespace

sock_fprog* IcmpFilter::getPacketFilter() const
{
    return &bpf;
}
} // namespace net