#pragma once

#include <memory>
#include <string_view>

namespace net
{

class NetworkInterface;

namespace v2
{
class IcmpSocket;
class RawSocket;
}

class ISocketPool
{
  public:
    virtual ~ISocketPool() = default;
    virtual std::shared_ptr<v2::IcmpSocket> createIcmpSocketv2() = 0;
    virtual std::shared_ptr<v2::RawSocket> createRawSocket(std::string_view interface) = 0;
};
} // namespace net
