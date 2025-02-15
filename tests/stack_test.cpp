#include <gtest/gtest.h>
#include "stack.h"

using namespace gtr::containers;

TEST(StackTest, DefaultConstructor) {
	stack<int> stack;
	EXPECT_EQ(stack.data.size(), 0);
	EXPECT_EQ(stack.data.capacity(), 0);
}

TEST(StackTest, Push) {
	stack<int> stack;
	stack.push(1);
	EXPECT_EQ(stack.data.size(), 1);
	EXPECT_EQ(stack.data.capacity(), 1);
	EXPECT_EQ(stack.data[0], 1);

	stack.push(2);
	EXPECT_EQ(stack.data.size(), 2);
	EXPECT_EQ(stack.data.capacity(), 2);
	EXPECT_EQ(stack.data[0], 1);
	EXPECT_EQ(stack.data[1], 2);

	stack.push(3);
	EXPECT_EQ(stack.data.size(), 3);
	EXPECT_EQ(stack.data.capacity(), 4);
	EXPECT_EQ(stack.data[0], 1);
	EXPECT_EQ(stack.data[1], 2);
	EXPECT_EQ(stack.data[2], 3);
}

TEST(StackTest, Pop) {
	stack<int> stack;
	stack.push(1);
	stack.push(2);
	stack.push(3);

	stack.pop();
	EXPECT_EQ(stack.data.size(), 2);
	EXPECT_EQ(stack.data.capacity(), 4);
	EXPECT_EQ(stack.data[0], 1);
	EXPECT_EQ(stack.data[1], 2);

	stack.pop();
	EXPECT_EQ(stack.data.size(), 1);
	EXPECT_EQ(stack.data.capacity(), 4);
	EXPECT_EQ(stack.data[0], 1);

	stack.pop();
	EXPECT_EQ(stack.data.size(), 0);
	EXPECT_EQ(stack.data.capacity(), 4);
}

TEST(StackTest, Top) {
	stack<int> stack;
	stack.push(1);
	stack.push(2);
	stack.push(3);

	EXPECT_EQ(stack.top(), 3);
	stack.pop();
	EXPECT_EQ(stack.top(), 2);
	stack.pop();
	EXPECT_EQ(stack.top(), 1);
	stack.pop();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}