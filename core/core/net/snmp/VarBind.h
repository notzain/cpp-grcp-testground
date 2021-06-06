#pragma once

#include "Oid.h"
#include <optional>

namespace net
{
class VarBind
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

    bool hasValue() const { return m_value.has_value(); }
    const std::optional<OidValue>& value() const { return m_value; }
    void setValue(OidValue value) { m_value = value; }
};
} // namespace net