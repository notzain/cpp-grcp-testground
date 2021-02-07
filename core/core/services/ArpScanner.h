#pragma once

#include "core/traits/Printable.h"
#include "core/traits/serializer/JsonSerializer.h"

namespace core::services
{
class ArpScanner
    : core::traits::JsonSerializer
    , core::traits::Printable<ArpScanner>
{
  public:
    void DoTest();

    nlohmann::json serialize(nlohmann::json json = {}) const override;

    std::string format() const override
    {
        return serialize().dump();
    }
};
} // namespace core::services