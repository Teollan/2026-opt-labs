#include <gtest/gtest.h>

#include <StringSource.hpp>

class StringSourceTest : public ::testing::Test {};

TEST_F(StringSourceTest, PointsToFirstCharAfterInit) {
    StringSource source("ABC");
    EXPECT_EQ(source.current(), 'A');
}

TEST_F(StringSourceTest, PointsToCorrectCharAfterOneRead) {
    StringSource source("ABC");
    char next = source.read();
    EXPECT_EQ(next, 'B');
    EXPECT_EQ(source.current(), 'B');
}

TEST_F(StringSourceTest, PointsToCorrectCharAfterMultipleReads) {
    StringSource source("ABCDE");
    source.read();
    source.read();
    char c = source.read();
    EXPECT_EQ(c, 'D');
    EXPECT_EQ(source.current(), 'D');
}

TEST_F(StringSourceTest, DoneIsFalseWhenCharsRemain) {
    StringSource source("AB");
    EXPECT_FALSE(source.done());
    source.read();
    EXPECT_FALSE(source.done());
}

TEST_F(StringSourceTest, DoneIsTrueAtEnd) {
    StringSource source("AB");
    source.read();
    source.read();
    EXPECT_TRUE(source.done());
}

TEST_F(StringSourceTest, DoneIsTrueForEmptyString) {
    StringSource source("");
    EXPECT_TRUE(source.done());
}

TEST_F(StringSourceTest, CurrentReturnsCorrectChar) {
    StringSource source("XYZ");
    EXPECT_EQ(source.current(), 'X');
    source.read();
    EXPECT_EQ(source.current(), 'Y');
    source.read();
    EXPECT_EQ(source.current(), 'Z');
}

TEST_F(StringSourceTest, TracksColumn) {
    StringSource source("ABC");
    EXPECT_EQ(source.column(), 0);
    source.read();
    EXPECT_EQ(source.column(), 1);
    source.read();
    EXPECT_EQ(source.column(), 2);
}

TEST_F(StringSourceTest, TracksRow) {
    StringSource source("A\nB\nC");
    EXPECT_EQ(source.row(), 0);
    source.read(); // '\n'
    source.read(); // 'B'
    EXPECT_EQ(source.row(), 1);
    source.read(); // '\n'
    source.read(); // 'C'
    EXPECT_EQ(source.row(), 2);
}

TEST_F(StringSourceTest, SingleCharInput) {
    StringSource source("X");
    EXPECT_FALSE(source.done());
    EXPECT_EQ(source.current(), 'X');
    source.read();
    EXPECT_TRUE(source.done());
}
