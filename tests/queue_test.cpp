#include <gtest/gtest.h>
#include "queue.h"

using namespace gtr::containers;

TEST(QueueTest, DefaultConstructor) {
	queue<int> queue;
	EXPECT_EQ(queue.list.size, 0);
}

TEST(QueueTest, Enqueue) {
	queue<int> queue;
	queue.enqueue(1);
	EXPECT_EQ(queue.list.size, 1);

	queue.enqueue(2);
	EXPECT_EQ(queue.list.size, 2);


	queue.enqueue(3);
	EXPECT_EQ(queue.list.size, 3);

}

TEST(QueueTest, Dequeue) {
	queue<int> queue;
	queue.enqueue(1);
	queue.enqueue(2);
	queue.enqueue(3);

	queue.dequeue();
	EXPECT_EQ(queue.list.size, 2);

	queue.dequeue();
	EXPECT_EQ(queue.list.size, 1);


	queue.dequeue();
	EXPECT_EQ(queue.list.size, 0);
}

TEST(QueueTest, Front) {
	queue<int> queue;
	queue.enqueue(1);
	queue.enqueue(2);
	queue.enqueue(3);

	EXPECT_EQ(queue.front(), 1);
	queue.dequeue();
	EXPECT_EQ(queue.front(), 2);
	queue.dequeue();
	EXPECT_EQ(queue.front(), 3);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}