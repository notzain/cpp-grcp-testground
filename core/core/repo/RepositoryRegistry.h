#pragma once

#include "Repository.h"
#include "core/util/Result.h"

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace repo
{
class RepositoryRegistry
{
    std::unordered_map<std::type_index, std::shared_ptr<IRepository>> m_repos;

  public:
    RepositoryRegistry() = default;

    static RepositoryRegistry& defaultRegistry()
    {
        static auto registry = RepositoryRegistry();
        return registry;
    }

    template <typename Repo>
    bool has()
    {
        return m_repos.count(typeid(Repo)) != 0;
    }

    template <typename Repo, typename... Args>
    Result<std::shared_ptr<Repo>> add(Args&&... args)
    {
        if (has<Repo>())
            return Error(ErrorType::AlreadyExists);

        auto repo = std::make_shared<Repo>(std::forward<Args>(args)...);
        m_repos[typeid(Repo)] = repo;
        return repo;
    }
    template <typename Repo>
    Result<std::shared_ptr<Repo>> get()
    {
        if (has<Repo>())
            return m_repos.at(typeid(Repo));

        return Error(ErrorType::NotFound);
    }
};
} // namespace repo