#pragma once
#include <memory>

namespace core::net
{

class ICMPSocket;
class RawSocket;
class NetworkInterface;

class ISocketPool
{
  public:
    virtual std::shared_ptr<ICMPSocket> createIcmpSocket() = 0;
    virtual std::shared_ptr<RawSocket> createRawSocket(NetworkInterface& networkInterface) = 0;
};
} // namespace core::net
