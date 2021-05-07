#pragma once

#include <memory>

namespace net
{

class NetworkInterface;

namespace v2
{
class IcmpSocket;
}

class ISocketPool
{
  public:
    virtual ~ISocketPool() = default;
    virtual std::shared_ptr<v2::IcmpSocket> createIcmpSocketv2() = 0;
};
} // namespace net
