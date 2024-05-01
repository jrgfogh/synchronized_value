#include "../synchronized_value/synchronized.h"

#include <shared_mutex>

#include "gtest_unwarn.h"

template <typename ValueType, typename MutexType>
concept valid_specialization =
    std::is_nothrow_move_assignable_v<sv::update_guard<ValueType, MutexType>> &&
    std::is_nothrow_move_constructible_v<
        sv::update_guard<ValueType, MutexType>> &&
    std::is_nothrow_swappable_v<sv::update_guard<ValueType, MutexType>> &&
    !std::is_copy_constructible_v<
        sv::synchronized<ValueType, MutexType>> &&
    std::is_same_v<typename sv::update_guard<ValueType, MutexType>::value_type,
                   ValueType> &&
    std::is_same_v<typename sv::update_guard<ValueType, MutexType>::mutex_type,
                   MutexType> &&
    std::is_same_v<
        typename sv::synchronized<ValueType, MutexType>::value_type,
        ValueType> &&
    std::is_same_v<
        typename sv::synchronized<ValueType, MutexType>::mutex_type,
        MutexType>;

static_assert(valid_specialization<int, std::mutex>);
static_assert(valid_specialization<int, std::shared_mutex>);
static_assert(valid_specialization<double, std::recursive_mutex>);

TEST(synchronized_value_tests, BasicAssertions) {
  sv::synchronized<std::map<int, int>> sv0;
  sv0.wlock()->emplace(0, 5);
  {
    sv::update_guard guard{sv0};
    ++(*guard)[0];
    auto var = guard->at(0);
  }
  sv::synchronized<std::tuple<int, int, int>> sv1{1, 33, 7};
  sv1.apply([](auto tuple) { auto [a, b, c] = tuple; });
}