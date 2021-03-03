#pragma once

#include <atomic>
#include <memory>
#include <string_view>

#include "IScanner.h"
#include "core/net/NetworkInterface.h"
#include "core/net/scanner/IScanner.h"
#include "core/net/socket/ICMPSocket.h"
#include "core/util/type/WrapAround.h"

namespace core::net
{
class ICMPScanner : public IAsyncScanner
{
    std::shared_ptr<ICMPSocket> m_socket;
    util::WrapAround<std::uint8_t> m_sequenceNumber;

  public:
    ICMPScanner(std::shared_ptr<ICMPSocket> socket);
    virtual ~ICMPScanner() = default;

    util::Result<int> ping(std::string_view host) override;
    void onPacketReceived(std::uint8_t* bytes, std::size_t len) override;
};
} // namespace core::net
