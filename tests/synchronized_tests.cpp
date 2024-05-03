#include "../synchronized_value/synchronized.h"

#include <shared_mutex>
#include <string>

#include "gtest_unwarn.h"

template <typename Synchronized>
concept rlock_uses_const =
    std::is_const_v<typename decltype(std::declval<Synchronized>().rlock())::value_type>;

template <typename Value, typename Mutex>
concept valid_specialization =
    std::is_nothrow_move_assignable_v<sv::update_guard<Value, Mutex>> &&
    std::is_nothrow_move_constructible_v<
        sv::update_guard<Value, Mutex>> &&
    std::is_nothrow_swappable_v<sv::update_guard<Value, Mutex>> &&
    !std::is_copy_constructible_v<
        sv::synchronized<Value, Mutex>> &&
    std::is_same_v<typename sv::update_guard<Value, Mutex>::value_type,
                   Value> &&
    std::is_same_v<typename sv::update_guard<Value, Mutex>::mutex_type,
                   Mutex> &&
    std::is_same_v<
        typename sv::synchronized<Value, Mutex>::value_type,
                   Value> &&
    std::is_same_v<
        typename sv::synchronized<Value, Mutex>::mutex_type,
                   Mutex> &&
    rlock_uses_const<sv::synchronized<Value, Mutex>>;

static_assert(valid_specialization<int, std::mutex>);
static_assert(valid_specialization<int, std::shared_mutex>);
static_assert(valid_specialization<double, std::recursive_mutex>);

TEST(synchronized_tests, ConstructorForwardsMoveOnlyType) {
  sv::synchronized<std::unique_ptr<int>> const sv0{std::make_unique<int>(5)};
  ASSERT_NE(sv0.rlock()->get(), nullptr);
  ASSERT_EQ(*sv0.rlock()->get(), 5);
}

TEST(synchronized_tests, ConstructorForwardsCopyOnlyType) {
  const std::shared_ptr<int> ptr = std::make_shared<int>(7);
  sv::synchronized<std::shared_ptr<int>> const sv1{ptr};
  ASSERT_NE(sv1.rlock()->get(), nullptr);
  ASSERT_EQ(*sv1.rlock()->get(), 7);
}

TEST(synchronized_tests, ConstructorIsVariadic) {
  sv::synchronized<std::pair<int, std::string>> const sv1{11, "example string literal"};
  ASSERT_EQ(sv1.rlock()->first, 11);
  ASSERT_EQ(sv1.rlock()->second, "example string literal");
}

TEST(synchronized_tests, BasicAssertions) {
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