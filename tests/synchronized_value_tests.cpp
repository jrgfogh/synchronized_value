#include "../synchronized_value/synchronized_value.h"

#include "gtest_unwarn.h"

static_assert(std::is_nothrow_move_assignable_v<update_guard<int>>);
static_assert(std::is_nothrow_move_constructible_v<update_guard<int>>);
static_assert(std::is_nothrow_swappable_v<update_guard<int>>);
static_assert(std::is_same_v<synchronized_value<int>::value_type,int>);

TEST(synchronized_value_tests, star_operator) {
  synchronized_value<int> sv{5};
  EXPECT_EQ(*sv, 5);
  *sv = 2;
  EXPECT_EQ(*sv, 2);
}

TEST(synchronized_value_tests, BasicAssertions) {
  synchronized_value<std::map<int, int>> sv0;
  sv0->emplace(0, 5);
  {
    update_guard guard{sv0};
    ++(*guard)[0];
    auto var = guard->at(0);
  }
  synchronized_value<std::tuple<int, int, int>> sv1{1, 33, 7};
  sv1.apply([](auto tuple) { auto [a, b, c] = tuple; });
}