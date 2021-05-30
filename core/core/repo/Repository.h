#pragma once

#include "core/util/Result.h"

namespace repo
{
class IRepository
{
    // empty base class so we can use polymorphic
};

template <typename T, typename K>
class Repository : public IRepository
{
  public:
    virtual bool create(const K& key, T&& item) = 0;
    virtual Result<T> read(const K& key) const = 0;
    virtual bool update(const K& key, T&& item) = 0;
    virtual bool remove(const K& key) = 0;
    virtual bool has(const K& key) const = 0;
};
} // namespace repo