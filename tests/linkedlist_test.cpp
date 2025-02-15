#include <gtest/gtest.h>
#include "linkedlist.h"

using namespace gtr::containers;

TEST(LinkedListTest, DefaultConstructor) {
	linked_list<int> list;
	EXPECT_EQ(list.size, 0);
	EXPECT_EQ(list.head, nullptr);
	EXPECT_EQ(list.tail, nullptr);
}

TEST(LinkedListTest, PushBack) {
	linked_list<int> list;
	list.push_back(1);
	EXPECT_EQ(list.size, 1);
	EXPECT_EQ(list.head->data, 1);
	EXPECT_EQ(list.tail->data, 1);

	list.push_back(2);
	EXPECT_EQ(list.size, 2);
	EXPECT_EQ(list.head->data, 1);
	EXPECT_EQ(list.tail->data, 2);

	list.push_back(3);
	EXPECT_EQ(list.size, 3);
	EXPECT_EQ(list.head->data, 1);
	EXPECT_EQ(list.tail->data, 3);
}

TEST(LinkedListTest, PushFront) {
	linked_list<int> list;
	list.push_front(1);
	EXPECT_EQ(list.size, 1);
	EXPECT_EQ(list.head->data, 1);
	EXPECT_EQ(list.tail->data, 1);

	list.push_front(2);
	EXPECT_EQ(list.size, 2);
	EXPECT_EQ(list.head->data, 2);
	EXPECT_EQ(list.tail->data, 1);

	list.push_front(3);
	EXPECT_EQ(list.size, 3);
	EXPECT_EQ(list.head->data, 3);
	EXPECT_EQ(list.tail->data, 1);
}

TEST(LinkedListTest, PopBack) {
	linked_list<int> list;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);

	list.pop_back();
	EXPECT_EQ(list.size, 2);
	EXPECT_EQ(list.head->data, 1);
	EXPECT_EQ(list.tail->data, 2);

	list.pop_back();
	EXPECT_EQ(list.size, 1);
	EXPECT_EQ(list.head->data, 1);
	EXPECT_EQ(list.tail->data, 1);

	list.pop_back();
	EXPECT_EQ(list.size, 0);
	EXPECT_EQ(list.head, nullptr);
	EXPECT_EQ(list.tail, nullptr);
}

TEST(LinkedListTest, PopFront) {
	linked_list<int> list;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);

	list.pop_front();
	EXPECT_EQ(list.size, 2);
	EXPECT_EQ(list.head->data, 2);
	EXPECT_EQ(list.tail->data, 3);

	list.pop_front();
	EXPECT_EQ(list.size, 1);
	EXPECT_EQ(list.head->data, 3);
	EXPECT_EQ(list.tail->data, 3);

	list.pop_front();
	EXPECT_EQ(list.size, 0);
	EXPECT_EQ(list.head, nullptr);
	EXPECT_EQ(list.tail, nullptr);
}

TEST(LinkedListTest, Iterator) {
	linked_list<int> list;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);

	int value = 1;
	for (const auto& item : list) {
		EXPECT_EQ(item, value);
		value++;
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}