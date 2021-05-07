#pragma once

#include <atomic>
#include <boost/core/noncopyable.hpp>
#include <string_view>
#include <thread>
#include <type_traits>

namespace util
{
class Thread : private boost::noncopyable
{
  public:
    enum StopTokenValue
    {
        Continue,
        Stop,
    };
    using StopToken = std::atomic<StopTokenValue>;

  private:
    std::thread m_thread;
    StopToken m_stopToken = { Thread::Continue };

  public:
    template <typename Func>
    [[nodiscard]] static Thread spawn(Func&& f)
    {
        return Thread(std::move(f));
    }

    template <typename Func>
    [[nodiscard]] static Thread spawn(std::string_view name, Func&& f)
    {
        return Thread(name, std::move(f));
    }

    virtual ~Thread();

    void requestStop();

  private:
    template <typename Func>
    Thread(std::string_view name, Func&& f)
        : m_thread([this, func = std::move(f)] {
            if constexpr (std::is_invocable_v<Func, const StopToken&>)
            {
                func(m_stopToken);
            }
            else
            {
                func();
            }
        })
    {
        if (!name.empty())
        {
            setName(name);
        }
    }

    void setName(std::string_view name);
};

static_assert(not std::is_move_assignable_v<Thread>, "Thread should not be movable.");
static_assert(not std::is_copy_assignable_v<Thread>, "Thread should not be copyable.");
} // namespace util

using util::Thread;