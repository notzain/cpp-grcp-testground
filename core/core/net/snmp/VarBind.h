#pragma once

#include "Oid.h"
#include "core/util/helper/Variant.h"
#include <optional>

namespace net
{
class VarBind : public traits::Printable<VarBind>
{
    Oid m_oid;
    std::optional<OidValue> m_value;

  public:
    VarBind(Oid oid)
        : m_oid(std::move(oid))
    {
    }

    VarBind(Oid oid, OidValue value)
        : m_oid(std::move(oid))
        , m_value(std::move(value))
    {
    }

    const Oid& oid() const { return m_oid; }

    const std::optional<OidValue>& value() const { return m_value; }
    void setValue(OidValue value) { m_value = value; }

    std::string format() const override
    {
        if (m_value)
        {
            std::string out;
            matchVariant(*m_value, [&](const auto& val) {
                out = fmt::format("{}: {}", m_oid, val);
            });
            return out;
        }
        else
        {
            return fmt::format("{}: None", m_oid);
        }
    }
};
} // namespace net