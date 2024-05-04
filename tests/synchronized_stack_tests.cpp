#include "../examples/synchronized_stack.h"

#include "gtest_unwarn.h"

static_assert(std::is_same_v<typename example::synchronized_stack<int>::value_type, int>);
static_assert(std::is_same_v<typename example::synchronized_stack<double>::value_type, double>);

TEST(synchronized_stack_tests, BasicUsage) {
  example::synchronized_stack<int> ss;
  EXPECT_TRUE(ss.empty());
  EXPECT_EQ(ss.size(), 0);
  ss.push(5);
  EXPECT_EQ(ss.size(), 1);
  ss.push(7);
  EXPECT_EQ(ss.size(), 2);
  EXPECT_FALSE(ss.empty());
  EXPECT_EQ(ss.pop(), 7);
  EXPECT_EQ(ss.pop(), 5);
  EXPECT_TRUE(ss.empty());
}