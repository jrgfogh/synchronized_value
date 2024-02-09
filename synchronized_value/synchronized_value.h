#pragma once

#include <mutex>

template <typename GuardedType>
class synchronized_value;

template <typename GuardedType>
class update_guard
{
	std::unique_lock<std::mutex> lock_;
	GuardedType &guarded_data_;
public:
	explicit update_guard(synchronized_value<GuardedType>& sv) :
		lock_{sv.mutex_},
		guarded_data_{sv.guarded_data_}
	{
	}

	auto* operator->() noexcept
	{
		return &guarded_data_;
	}

	auto& operator*() noexcept
	{
		return guarded_data_;
	}
};

template <typename GuardedType>
class synchronized_value
{
	friend class update_guard<GuardedType>;

	std::mutex mutex_;
	GuardedType guarded_data_;
public:
	synchronized_value(synchronized_value const&) = delete;
	synchronized_value& operator=(synchronized_value const&) = delete;

	template<typename ... Args>
	explicit synchronized_value(Args&& ... args) :
		guarded_data_{std::forward<Args>(args)...}
	{
	}

	template<typename F>
	auto apply(F&& func)
	{
		std::unique_lock lock{mutex_};
		func(guarded_data_);
	}

	auto operator->()
	{
		return update_guard{*this};
	}

	auto operator*()
	{
		class value_locker
		{
			std::unique_lock<std::mutex> lock_;
			GuardedType &guarded_data_;
		public:
			explicit value_locker(std::mutex& mutex, GuardedType &data) :
				lock_{mutex},
				guarded_data_{data}
			{
			}

			explicit(false) operator GuardedType&()
			{
				return guarded_data_;
			}
		};
		return value_locker{mutex_, guarded_data_};
	}
};
