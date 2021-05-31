#pragma once

#include "Repository.h"
#include "core/util/Result.h"

#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace repo
{
class RepositoryRegistry
{
    std::unordered_map<std::type_index, std::shared_ptr<IRepository>> m_repos;

    template <typename Iface>
    class Register
    {
        RepositoryRegistry& registry;

      public:
        Register(RepositoryRegistry& r)
            : registry(r)
        {
        }

        template <typename Impl, typename... Args>
        Result<std::shared_ptr<Impl>> with(Args&&... args)
        {
            static_assert(std::is_base_of_v<Iface, Impl>, "Injection Impl should be derived from Iface");

            if (registry.has<Iface>())
                return Error(ErrorType::AlreadyExists);

            auto repo = std::make_shared<Impl>(std::forward<args>(args)...);
            registry.m_repos.emplace(typeid(Iface), repo);
            return repo;
        }

        template <typename Impl>
        Result<std::shared_ptr<Impl>> registerRepository(std::shared_ptr<Impl> repo)
        {
            static_assert(std::is_base_of_v<Iface, Impl>, "Injection Impl should be derived from Iface");

            if (registry.has<Iface>())
                return Error(ErrorType::AlreadyExists);

            registry.m_repos.emplace(typeid(Iface), repo);
            return repo;
        }
    };

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

    template <typename Repo>
    Result<std::shared_ptr<Repo>> get()
    {
        if (has<Repo>())
            return m_repos.at(typeid(Repo));

        return Error(ErrorType::NotFound);
    }

    template <typename Repo>
    Result<std::shared_ptr<Repo>> registerRepository(std::shared_ptr<Repo> repo)
    {
        if (has<Repo>())
            return Error(ErrorType::AlreadyExists);

        m_repos.emplace(typeid(Repo), repo);
        return repo;
    }

    template <typename Repo, typename... Args>
    Result<std::shared_ptr<Repo>> registerRepository(Args&&... args)
    {
        if (has<Repo>())
            return Error(ErrorType::AlreadyExists);

        auto repo = std::make_shared<Repo>(std::forward<args>(args)...);
        m_repos.emplace(typeid(Repo), repo);
        return repo;
    }

    template <typename RepoInterface>
    Register<RepoInterface> injectRepository()
    {
        return Register<RepoInterface>(*this);
    }
};
} // namespace repo