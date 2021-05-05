#pragma once

#include "core/util/Result.h"

#include <cstdint>
#include <nonstd/span.hpp>

namespace net::v2
{
template <typename Protocol>
class Socket
{
    using Endpoint = typename Protocol::Endpoint;

  public:
    virtual ~Socket() = default;
    virtual bool connect() = 0;
    virtual bool isConnected() const = 0;

    virtual bool disconnect() = 0;

    virtual Result<std::size_t> send(nonstd::span<std::uint8_t> data) = 0;
    virtual Result<std::size_t> sendTo(const Endpoint& endpoint, nonstd::span<std::uint8_t> data) = 0;
};
} // namespace net::v2