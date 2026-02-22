#include <gtest/gtest.h>

#include <SourcePosition.hpp>

TEST(SourcePositionTest, StartsAtZeroZero) {
    SourcePosition position;
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 0);
}

TEST(SourcePositionTest, AdvancesColumnForNormalCharacters) {
    SourcePosition position;
    position.advance('A');
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 1);

    position.advance('B');
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 2);
}

TEST(SourcePositionTest, AdvancesRowAndResetsColumnForNewline) {
    SourcePosition position;
    position.advance('A');
    position.advance('B');
    position.advance('\n');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 0);
}

TEST(SourcePositionTest, AdvancesColumnByTabSize) {
    SourcePosition position;
    position.advance('\t');
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 4);

    position.advance('\t');
    EXPECT_EQ(position.column(), 8);
}

TEST(SourcePositionTest, AdvancesColumnByTabSize2) {
    SourcePosition position(2);
    position.advance('\t');
    EXPECT_EQ(position.column(), 2);
    position.advance('\t');
    EXPECT_EQ(position.column(), 4);
}

TEST(SourcePositionTest, AdvancesColumnByTabSize8) {
    SourcePosition position(8);
    position.advance('\t');
    EXPECT_EQ(position.column(), 8);
    position.advance('\t');
    EXPECT_EQ(position.column(), 16);
}

TEST(SourcePositionTest, HandlesMultipleAdvancesCorrectly) {
    SourcePosition position;
    // "AB\nCD\tE"
    position.advance('A');
    position.advance('B');
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 2);

    position.advance('\n');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 0);

    position.advance('C');
    position.advance('D');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 2);

    position.advance('\t');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 6);

    position.advance('E');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 7);
}
