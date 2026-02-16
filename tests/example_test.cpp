#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(ArithmeticTest, Addition) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_EQ(-1 + 1, 0);
}

TEST(ArithmeticTest, Multiplication) {
    EXPECT_EQ(3 * 7, 21);
    EXPECT_EQ(0 * 42, 0);
}

TEST(StringTest, Concatenation) {
    std::string hello = "Hello";
    std::string world = "SIGNAL";
    EXPECT_EQ(hello + ", " + world + "!", "Hello, SIGNAL!");
}

TEST(StringTest, Length) {
    std::string signal = "SIGNAL";
    EXPECT_EQ(signal.size(), 6);
    EXPECT_FALSE(signal.empty());
}

// --- Fixture example (like beforeEach/afterEach in vitest) ---

class VectorTest : public ::testing::Test {
protected:
    std::vector<int> vec;

    void SetUp() override {
        // Runs before each TEST_F — equivalent to beforeEach()
        vec = {1, 2, 3, 4, 5};
    }

    // TearDown() override is available too, but not needed here
    // since vector cleans up automatically
};

TEST_F(VectorTest, InitialSize) {
    EXPECT_EQ(vec.size(), 5);
}

TEST_F(VectorTest, PushBack) {
    vec.push_back(6);
    EXPECT_EQ(vec.size(), 6);
    EXPECT_EQ(vec.back(), 6);
}

TEST_F(VectorTest, PopBack) {
    vec.pop_back();
    EXPECT_EQ(vec.size(), 4);
    EXPECT_EQ(vec.back(), 4);
}
