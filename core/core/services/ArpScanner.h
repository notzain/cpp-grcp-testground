#pragma once

#include "core/traits/Printable.h"
#include "core/traits/serializer/JsonSerializer.h"

namespace services
{
class ArpScanner
    : traits::JsonSerializer
    , traits::Printable<ArpScanner>
{
  public:
    void DoTest();

    nlohmann::json serialize(nlohmann::json json = {}) const override;

    std::string format() const override
    {
        return serialize().dump();
    }
};
} // namespace services