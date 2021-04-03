#pragma once

#include <nonstd/expected.hpp>
#include <string>

namespace net
{
class ISocket
{
  public:
    virtual ~ISocket() = default;
    virtual bool connect() = 0;
    virtual bool isConnected() const = 0;
    virtual bool disconnect() = 0;
    virtual void permitBroadcast(bool permit) = 0;
    virtual nonstd::expected<std::size_t, std::string> send(void* data, std::size_t len) = 0;
    virtual nonstd::expected<std::size_t, std::string> sendTo(std::string_view host, std::size_t port, void* data, std::size_t len) = 0;
};
} // namespace net
