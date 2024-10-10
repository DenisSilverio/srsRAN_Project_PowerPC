/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/noop_functor.h"
#include "srsran/support/async/execute_on.h"
#include "srsran/support/timers.h"

namespace srsran {

namespace detail {

template <typename TaskExecutor, typename OnFailureToDispatch, bool IsExecute>
auto dispatch_on_blocking(TaskExecutor& exec, timer_manager& timers, OnFailureToDispatch&& on_failure)
{
  struct blocking_dispatch_on_awaiter {
    blocking_dispatch_on_awaiter(TaskExecutor& exec_, timer_manager& timers_, OnFailureToDispatch&& on_failure_) :
      exec(exec_), timers(timers_), on_failure(std::forward<OnFailureToDispatch>(on_failure_))
    {
    }

    bool await_ready() noexcept { return false; }

    void await_suspend(coro_handle<> suspending_awaitable)
    {
      auto task = [suspending_awaitable]() mutable { suspending_awaitable.resume(); };

      // Try to dispatch task.
      if constexpr (IsExecute) {
        if (exec.execute(task)) {
          return;
        }
      } else {
        if (exec.defer(task)) {
          return;
        }
      }

      // Task execute/defer failed (potentially because task executor queue is full).
      on_failure();

      // Leverage timer infrastructure to run task.
      // Note: Even if the timer expiry fails to invoke task in executor, it keeps trying on every tick.
      retry_timer = timers.create_unique_timer(exec);
      retry_timer.set(std::chrono::milliseconds{1},
                      [suspending_awaitable](timer_id_t tid) mutable { suspending_awaitable.resume(); });
      retry_timer.run();
    }

    void await_resume() {}

    blocking_dispatch_on_awaiter& get_awaiter() { return *this; }

  private:
    TaskExecutor&       exec;
    timer_manager&      timers;
    OnFailureToDispatch on_failure;
    unique_timer        retry_timer;
  };

  return blocking_dispatch_on_awaiter{exec, timers, std::forward<OnFailureToDispatch>(on_failure)};
}

} // namespace detail

/// \brief Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call
/// to execute fails, the awaitable yields and will retry the dispatch at a later point, until it succeeds.
/// \param[in] exec Executor used to dispatch coroutine to a new execution context.
/// \param[in] timers Timer service used to handle reattempts to dispatch task to new execution context.
/// \param[in] on_failure Callback invoked in case the dispatch to executor fails at first attempt.
template <typename TaskExecutor, typename OnFailureToDispatch = noop_operation>
auto execute_on_blocking(TaskExecutor& exec, timer_manager& timers, OnFailureToDispatch&& on_failure = noop_operation{})
{
  return detail::dispatch_on_blocking<TaskExecutor, OnFailureToDispatch, true>(
      exec, timers, std::forward<OnFailureToDispatch>(on_failure));
}

/// \brief Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call
/// to defer fails, the awaitable yields and will retry the dispatch at a later point, until it succeeds.
/// \param[in] exec Executor used to dispatch coroutine to a new execution context.
/// \param[in] timers Timer service used to handle reattempts to dispatch task to new execution context.
/// \param[in] on_failure Callback invoked in case the dispatch to executor fails at first attempt.
template <typename TaskExecutor, typename OnFailureToDispatch = noop_operation>
auto defer_on_blocking(TaskExecutor& exec, timer_manager& timers, OnFailureToDispatch&& on_failure = noop_operation{})
{
  return detail::dispatch_on_blocking<TaskExecutor, OnFailureToDispatch, false>(
      exec, timers, std::forward<OnFailureToDispatch>(on_failure));
}

/// \brief Returns an async_task<ReturnType> that runs a given invocable task in a \c dispatch_exec executor, and once
/// the task is complete, it resumes the suspended coroutine in a \c return_exec executor.
template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename OnFailureToDispatch = noop_operation>
auto execute_and_continue_on_blocking(DispatchTaskExecutor& dispatch_exec,
                                      CurrentTaskExecutor&  return_exec,
                                      timer_manager&        timers,
                                      Callable&&            callable,
                                      OnFailureToDispatch&& on_failure = noop_operation{})
{
  using return_type = detail::function_return_t<decltype(&Callable::operator())>;

  if constexpr (std::is_same_v<return_type, void>) {
    // async_task<void> case.

    return launch_async([&return_exec,
                         &dispatch_exec,
                         task       = std::forward<Callable>(callable),
                         on_failure = std::forward<OnFailureToDispatch>(on_failure),
                         &timers](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);

      // Dispatch execution context switch.
      CORO_AWAIT(execute_on_blocking(dispatch_exec, timers, on_failure));

      // Run task.
      task();

      // Continuation in the original executor.
      CORO_AWAIT(execute_on_blocking(return_exec, timers, on_failure));

      CORO_RETURN();
    });

  } else {
    // async_task<return_type> case.

    return launch_async([&return_exec,
                         &dispatch_exec,
                         task       = std::forward<Callable>(callable),
                         on_failure = std::forward<OnFailureToDispatch>(on_failure),
                         &timers,
                         ret = return_type{}](coro_context<async_task<return_type>>& ctx) mutable {
      CORO_BEGIN(ctx);

      // Dispatch execution context switch.
      CORO_AWAIT(execute_on_blocking(dispatch_exec, timers, on_failure));

      // Run task.
      ret = task();

      // Continuation in the original executor.
      CORO_AWAIT(execute_on_blocking(return_exec, timers, on_failure));

      CORO_RETURN(ret);
    });
  }
}

} // namespace srsran