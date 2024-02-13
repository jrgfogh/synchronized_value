#include "../examples/doubly_linked_list.h"

#include "gtest_unwarn.h"

TEST(doubly_linked_list, iterate_empty_list) {
	doubly_linked_list<int> l;
	for (int d : l) {}
}

TEST(doubly_linked_list, insert_element_and_iterate) {
	doubly_linked_list<int> l;
	l.insert(l.end(), 1);
	EXPECT_EQ(l.size(), 1);
	for (int d : l)
		EXPECT_EQ(d, 1);
}

TEST(doubly_linked_list, insert_three_elements_and_iterate) {
	doubly_linked_list<int> l;
	l.insert(l.end(), 1);
	l.insert(l.end(), 2);
	l.insert(l.end(), 3);
	EXPECT_EQ(l.size(), 3);
	std::vector<int> output;
	for (int d : l)
		output.emplace_back(d);
	EXPECT_EQ(output, (std::vector<int>{1, 2, 3}));
}

TEST(doubly_linked_list, insert_three_elements_in_reverse_and_iterate) {
	doubly_linked_list<int> l;
	auto next = l.insert(l.end(), 1);
	next = l.insert(next , 2);
	l.insert(next , 3);
	EXPECT_EQ(l.size(), 3);
	std::vector<int> output;
	for (int d : l)
		output.emplace_back(d);
	EXPECT_EQ(output, (std::vector<int>{3, 2, 1}));
}

TEST(doubly_linked_list, insert_five_elements_out_of_order_and_iterate) {
	doubly_linked_list<int> l;
	auto next = l.insert(l.end(), 1);
	l.insert(next, 2);
	next = l.insert(l.end(), 3);
	l.insert(next, 4);
	l.insert(next, 5);
	EXPECT_EQ(l.size(), 5);
	std::vector<int> output;
	for (int d : l)
		output.emplace_back(d);
	EXPECT_EQ(output, (std::vector{2, 1, 4, 5, 3}));
}