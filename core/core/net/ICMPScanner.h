#pragma once

#include <memory>
#include <string_view>

#include "NetworkInterface.h"
#include "core/net/socket/ICMPSocket.h"
#include "core/util/type/WrapAround.h"
#include "core/util/Result.h"

namespace core::net
{
class ICMPScanner
{
    NetworkInterface& m_networkInterface;
    std::shared_ptr<ICMPSocket> m_socket;
    core::util::WrapAround<std::uint8_t> m_sequenceNumber;

  public:
    ICMPScanner(NetworkInterface& networkInterface, std::shared_ptr<ICMPSocket> socket);
    virtual core::util::Result<int>  ping(std::string_view host);

};
} // namespace core::net