#include "../examples/doubly_linked_list.h"

#include "gtest_unwarn.h"

TEST(doubly_linked_list, iterate_empty_list) {
	doubly_linked_list<int> l;
	for (int d : l) {}
}