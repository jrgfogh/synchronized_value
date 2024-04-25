#pragma once

#include <mutex>

template <typename GuardedType> class synchronized_value;

template <typename GuardedType> class update_guard {
  std::unique_lock<std::mutex> lock_;
  GuardedType *guarded_data_;

public:
  explicit update_guard(synchronized_value<GuardedType> &sv)
      : lock_{sv.mutex_}, guarded_data_{&sv.guarded_data_} {}

  auto operator->() noexcept -> GuardedType * { return guarded_data_; }

  auto operator*() noexcept -> GuardedType & { return *guarded_data_; }
};

template <typename GuardedType> class synchronized_value {
  friend class update_guard<GuardedType>;

  std::mutex mutex_;
  GuardedType guarded_data_;

public:

  using value_type = GuardedType;

  synchronized_value(synchronized_value const &) = delete;
  auto operator=(synchronized_value const &) -> synchronized_value & = delete;

  template <typename... Args>
  explicit synchronized_value(Args &&...args)
      : guarded_data_{std::forward<Args>(args)...} {}

  template <typename F> auto apply(F const &func) {
    std::unique_lock lock{mutex_};
    return func(guarded_data_);
  }

  auto operator->() { return update_guard{*this}; }

  auto operator*() {
    class value_locker {
      std::unique_lock<std::mutex> lock_;
      GuardedType &guarded_data_;

    public:
      explicit value_locker(std::mutex &mutex, GuardedType &data)
          : lock_{mutex}, guarded_data_{data} {}

      explicit(false) operator GuardedType &() { return guarded_data_; }

      explicit(false) operator GuardedType const &() const {
        return guarded_data_;
      }

      auto operator=(GuardedType const &new_data) -> value_locker & {
        guarded_data_ = new_data;
        return *this;
      }
    };
    return value_locker{mutex_, guarded_data_};
  }
};
