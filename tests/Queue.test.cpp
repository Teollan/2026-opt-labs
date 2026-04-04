#include <gtest/gtest.h>

#include <Queue.hpp>

class QueueTest : public ::testing::Test {};

TEST_F(QueueTest, StartsEmpty) {
    Queue<int> queue;

    EXPECT_TRUE(queue.isEmpty());
    EXPECT_EQ(queue.size(), 0);
}

TEST_F(QueueTest, PushAndPeekReturnsFront) {
    Queue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(queue.peek(), 1);
    EXPECT_EQ(queue.size(), 3);
}

TEST_F(QueueTest, PopReturnsFrontInFIFOOrder) {
    Queue<int> queue;
    queue.push(10);
    queue.push(20);
    queue.push(30);

    EXPECT_EQ(queue.pop(), 10);
    EXPECT_EQ(queue.pop(), 20);
    EXPECT_EQ(queue.pop(), 30);
}

TEST_F(QueueTest, PopReturnsNulloptWhenEmpty) {
    Queue<int> queue;

    EXPECT_EQ(queue.pop(), std::nullopt);
}

TEST_F(QueueTest, PeekReturnsNulloptWhenEmpty) {
    Queue<int> queue;

    EXPECT_EQ(queue.peek(), std::nullopt);
}

TEST_F(QueueTest, ConstructsFromVector) {
    Queue<int> queue({1, 2, 3});

    EXPECT_EQ(queue.pop(), 1);
    EXPECT_EQ(queue.pop(), 2);
    EXPECT_EQ(queue.pop(), 3);
}

TEST_F(QueueTest, ClearRemovesAllItems) {
    Queue<int> queue({1, 2, 3});
    queue.clear();

    EXPECT_TRUE(queue.isEmpty());
    EXPECT_EQ(queue.pop(), std::nullopt);
}

TEST_F(QueueTest, PopUntilFindsMatchingItem) {
    Queue<int> queue({1, 2, 3, 4, 5});

    auto found = queue.popUntil([](int item) { return item == 3; });

    EXPECT_EQ(found, 3);
    // Items before 3 were discarded, 3 is still in the queue
    EXPECT_EQ(queue.peek(), 3);
}

TEST_F(QueueTest, PopUntilReturnsNulloptWhenNoMatch) {
    Queue<int> queue({1, 2, 3});

    auto found = queue.popUntil([](int item) { return item == 99; });

    EXPECT_EQ(found, std::nullopt);
    EXPECT_TRUE(queue.isEmpty());
}

TEST_F(QueueTest, PopUntilDiscardsItemsBeforeMatch) {
    Queue<int> queue({10, 20, 30, 40});

    queue.popUntil([](int item) { return item == 30; });

    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.pop(), 30);
    EXPECT_EQ(queue.pop(), 40);
}

TEST_F(QueueTest, PeekDoesNotConsumeItem) {
    Queue<int> queue({42});

    EXPECT_EQ(queue.peek(), 42);
    EXPECT_EQ(queue.peek(), 42);
    EXPECT_EQ(queue.size(), 1);
}

TEST_F(QueueTest, SizeTracksCorrectly) {
    Queue<int> queue;

    EXPECT_EQ(queue.size(), 0);
    queue.push(1);
    EXPECT_EQ(queue.size(), 1);
    queue.push(2);
    EXPECT_EQ(queue.size(), 2);
    queue.pop();
    EXPECT_EQ(queue.size(), 1);
}
