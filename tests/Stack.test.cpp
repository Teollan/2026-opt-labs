#include <gtest/gtest.h>

#include <Stack.hpp>

class StackTest : public ::testing::Test {};

// Constructor

TEST_F(StackTest, DefaultConstructorCreatesEmptyStack) {
    Stack<int> stack;
    EXPECT_TRUE(stack.isEmpty());
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(StackTest, VectorConstructorCopiesItems) {
    Stack<int> stack({1, 2, 3});
    EXPECT_EQ(stack.size(), 3);
    EXPECT_FALSE(stack.isEmpty());
}

TEST_F(StackTest, VectorConstructorLastElementIsOnTop) {
    Stack<int> stack({1, 2, 3});
    EXPECT_EQ(stack.peek(), 3);
}

// push

TEST_F(StackTest, PushIncreasesSize) {
    Stack<int> stack;
    stack.push(1);
    EXPECT_EQ(stack.size(), 1);
    stack.push(2);
    EXPECT_EQ(stack.size(), 2);
}

TEST_F(StackTest, PushAddsToTop) {
    Stack<int> stack;
    stack.push(1);
    stack.push(2);
    EXPECT_EQ(stack.peek(), 2);
}

// pop

TEST_F(StackTest, PopReturnsTopElement) {
    Stack<int> stack({1, 2, 3});
    EXPECT_EQ(stack.pop(), 3);
}

TEST_F(StackTest, PopRemovesTopElement) {
    Stack<int> stack({1, 2, 3});
    stack.pop();
    EXPECT_EQ(stack.peek(), 2);
    EXPECT_EQ(stack.size(), 2);
}

TEST_F(StackTest, PopReturnsNulloptOnEmptyStack) {
    Stack<int> stack;
    EXPECT_EQ(stack.pop(), std::nullopt);
}

TEST_F(StackTest, PopAllElementsInLifoOrder) {
    Stack<int> stack({1, 2, 3});
    EXPECT_EQ(stack.pop(), 3);
    EXPECT_EQ(stack.pop(), 2);
    EXPECT_EQ(stack.pop(), 1);
    EXPECT_TRUE(stack.isEmpty());
}

// peek

TEST_F(StackTest, PeekReturnsTopWithoutRemoving) {
    Stack<int> stack({1, 2});
    EXPECT_EQ(stack.peek(), 2);
    EXPECT_EQ(stack.size(), 2);
}

TEST_F(StackTest, PeekReturnsNulloptOnEmptyStack) {
    Stack<int> stack;
    EXPECT_EQ(stack.peek(), std::nullopt);
}

// isEmpty

TEST_F(StackTest, IsEmptyAfterPoppingAllElements) {
    Stack<int> stack;
    stack.push(1);
    stack.pop();
    EXPECT_TRUE(stack.isEmpty());
}

// Works with strings

TEST_F(StackTest, WorksWithStrings) {
    Stack<std::string> stack({"hello", "world"});
    EXPECT_EQ(stack.pop(), "world");
    EXPECT_EQ(stack.pop(), "hello");
}
