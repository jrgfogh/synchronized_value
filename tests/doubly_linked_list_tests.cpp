#include "../examples/concurrent_doubly_linked_list.h"
#include "../examples/doubly_linked_list.h"

#include "gtest_unwarn.h"

#include <algorithm>
#include <execution>
#include <future>
#include <random>

#include "../synchronized_value/synchronized_value.h"

template <typename DoublyLinkedList>
class doubly_linked_list_tests : public ::testing::Test {};

using list_types =
    ::testing::Types<example::doubly_linked_list<int>,
                     example::concurrent_doubly_linked_list<int>>;
TYPED_TEST_SUITE(doubly_linked_list_tests, list_types);

TYPED_TEST(doubly_linked_list_tests, iterate_empty_list) {
  for (const TypeParam l; int _ : l) {
  }
}

TYPED_TEST(doubly_linked_list_tests, insert_element_and_iterate) {
  TypeParam l;
  l.insert(l.end(), 1);
  for (int d : l)
    EXPECT_EQ(d, 1);
  EXPECT_TRUE(l.check_invariants());
}

TYPED_TEST(doubly_linked_list_tests, insert_three_elements_and_iterate) {
  TypeParam l;
  l.insert(l.end(), 1);
  l.insert(l.end(), 2);
  l.insert(l.end(), 3);
  std::vector<int> output;
  for (int d : l)
    output.emplace_back(d);
  EXPECT_EQ(output, (std::vector<int>{1, 2, 3}));
  EXPECT_TRUE(l.check_invariants());
}

TYPED_TEST(doubly_linked_list_tests,
           insert_three_elements_in_reverse_and_iterate) {
  TypeParam l;
  auto next = l.insert(l.end(), 1);
  next = l.insert(next, 2);
  l.insert(next, 3);
  std::vector<int> output;
  for (int d : l)
    output.emplace_back(d);
  EXPECT_EQ(output, (std::vector<int>{3, 2, 1}));
  EXPECT_TRUE(l.check_invariants());
}

TYPED_TEST(doubly_linked_list_tests,
           insert_five_elements_out_of_order_and_iterate) {
  TypeParam l;
  auto next = l.insert(l.end(), 1);
  l.insert(next, 2);
  next = l.insert(l.end(), 3);
  l.insert(next, 4);
  l.insert(next, 5);
  std::vector<int> output;
  for (int d : l)
    output.emplace_back(d);
  EXPECT_EQ(output, (std::vector{2, 1, 4, 5, 3}));
  EXPECT_TRUE(l.check_invariants());
}

TYPED_TEST(doubly_linked_list_tests, insert_and_erase_elements) {
  TypeParam l;
  l.insert(l.end(), 1);
  typename TypeParam::iterator next = l.insert(l.end(), 2);
  l.insert(l.end(), 3);
  next = l.erase(next);
  EXPECT_EQ(*next, 3);
  next = l.erase(l.begin());
  EXPECT_EQ(*next, 3);
  next = l.erase(l.begin());
  EXPECT_EQ(next, l.end());
  l.insert(l.end(), 5);
  EXPECT_TRUE(l.check_invariants());
}

TYPED_TEST(doubly_linked_list_tests, stress_test) {
  TypeParam l;
  std::vector<typename TypeParam::iterator> iterators;
  iterators.reserve(5000);
  for (int i = 0; i < 5000; i++)
    iterators.emplace_back(l.insert(l.end(), i));
  EXPECT_TRUE(l.check_invariants());
  std::mt19937 mt; // NOLINT(cert-msc51-cpp)
  std::ranges::shuffle(iterators, mt);
  for (auto const it : iterators)
    l.erase(it);
  EXPECT_TRUE(l.check_invariants());
}

TEST(doubly_linked_list_tests, async) {
  example::doubly_linked_list<int> l;
  std::vector<example::doubly_linked_list<int>::iterator> iterators;
  iterators.reserve(5000);
  for (int i = 0; i < 5000; i++)
    iterators.emplace_back(l.insert(l.end(), i));
  EXPECT_TRUE(l.check_invariants());
  std::mt19937 mt; // NOLINT(cert-msc51-cpp)
  std::ranges::shuffle(iterators, mt);
  std::vector<std::future<void>> futures;
  futures.reserve(iterators.size());
  synchronized_value<example::doubly_linked_list<int>> sync_l{std::move(l)};
  for (auto const it : iterators) {
    futures.emplace_back(std::async([&sync_l, it] {
      update_guard guard{sync_l};
      guard->erase(it);
      EXPECT_TRUE(guard->check_invariants());
    }));
  }
  for (auto &future : futures)
    future.get();
}