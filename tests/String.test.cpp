#include <gtest/gtest.h>

#include <String.hpp>

TEST(StringTest, RepeatZeroTimesReturnsEmptyString) {
    EXPECT_EQ(String::repeat("abc", 0), "");
}

TEST(StringTest, RepeatOneTimeReturnsSameString) {
    EXPECT_EQ(String::repeat("abc", 1), "abc");
}

TEST(StringTest, RepeatMultipleTimes) {
    EXPECT_EQ(String::repeat("ab", 3), "ababab");
}

TEST(StringTest, RepeatEmptyString) {
    EXPECT_EQ(String::repeat("", 5), "");
}

TEST(StringTest, RepeatSingleCharacter) {
    EXPECT_EQ(String::repeat("-", 4), "----");
}
