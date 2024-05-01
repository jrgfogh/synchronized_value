#pragma once

#include <mutex>

namespace sv {

template <typename L>
concept basic_lockable = requires(L m) {
  m.lock();
  m.unlock();
};

template <typename GuardedType, basic_lockable MutexType = std::mutex>
class synchronized;

template <typename GuardedType, basic_lockable MutexType = std::mutex>
class update_guard {
  std::unique_lock<MutexType> lock_;
  GuardedType *guarded_data_;

public:
  using value_type = GuardedType;
  using mutex_type = MutexType;

  explicit update_guard(synchronized<GuardedType, MutexType> &sv)
      : lock_{sv.mutex_}, guarded_data_{&sv.guarded_data_} {}

  auto operator->() noexcept -> GuardedType * { return guarded_data_; }

  auto operator*() noexcept -> GuardedType & { return *guarded_data_; }
};

template <typename GuardedType, basic_lockable MutexType> class synchronized {
  friend class update_guard<GuardedType, MutexType>;

  MutexType mutex_;
  GuardedType guarded_data_;

public:
  using value_type = GuardedType;
  using mutex_type = MutexType;

  synchronized(synchronized const &) = delete;
  auto operator=(synchronized const &) -> synchronized & = delete;

  template <typename... Args>
  explicit synchronized(Args &&...args)
      : guarded_data_{std::forward<Args>(args)...} {}

  template <typename F> auto apply(F const &func) {
    std::unique_lock lock{mutex_};
    return func(guarded_data_);
  }

  auto wlock() { return sv::update_guard{*this}; }
};
} // namespace sv
