#pragma once

#include "core/traits/Printable.h"

#include <boost/container_hash/hash.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cstddef>
#include <string>
#include <utility>

namespace util
{
class Uuid : public traits::Printable<Uuid>
{
    boost::uuids::uuid m_uuid;

  public:
    static Uuid generate()
    {
        return Uuid(boost::uuids::random_generator()());
    }

    static Uuid nil() { return Uuid(); }

    bool isNil() const { return *this == nil(); }

    std::size_t hash() const
    {
        boost::hash<boost::uuids::uuid> uuid_hasher;
        return uuid_hasher(m_uuid);
    }

    std::string toString() const override
    {
        return boost::uuids::to_string(m_uuid);
    }

    bool operator==(const Uuid& other) const { return m_uuid == other.m_uuid; }
    bool operator!=(const Uuid& other) const { return !(*this == other); }
    bool operator<(const Uuid& other) const { return m_uuid < other.m_uuid; }
    bool operator>(const Uuid& other) const { return other < *this; }
    bool operator<=(const Uuid& other) const { return !(other < *this); }
    bool operator>=(const Uuid& other) const { return !(*this < other); }

  private:
    Uuid() = default;

    Uuid(boost::uuids::uuid uuid)
        : m_uuid(std::move(uuid))
    {
    }
};
} // namespace util

using util::Uuid;