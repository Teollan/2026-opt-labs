#include <gtest/gtest.h>

#include <Cursor.hpp>

TEST(CursorTest, StartsAtZeroZero) {
    Cursor position;
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 0);
}

TEST(CursorTest, AdvancesColumnForNormalCharacters) {
    Cursor position;
    position.advance('A');
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 1);

    position.advance('B');
    EXPECT_EQ(position.row(), 0);
    EXPECT_EQ(position.column(), 2);
}

TEST(CursorTest, AdvancesRowAndResetsColumnForNewline) {
    Cursor position;
    position.advance('A');
    position.advance('B');
    position.advance('\n');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 0);
}

TEST(CursorTest, CRLFLineEndingsDoNotAffectColumn) {
    Cursor cursor;
    cursor.advance('A');
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 2);
    cursor.advance('\r');
    cursor.advance('\n');
    EXPECT_EQ(cursor.row(), 1);
    EXPECT_EQ(cursor.column(), 0);
    cursor.advance('C');
    EXPECT_EQ(cursor.column(), 1);
}

TEST(CursorTest, HandlesMultipleAdvancesCorrectly) {
    Cursor position;
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
    EXPECT_EQ(position.column(), 4);  // snaps from 2 to next tab stop

    position.advance('E');
    EXPECT_EQ(position.row(), 1);
    EXPECT_EQ(position.column(), 5);
}

// --- Tab stop snapping (tabSize = 4) ---

// "\tB" -> B at col 4
TEST(CursorTest, TabThenChar) {
    Cursor cursor;
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 4);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 5);
}

// "A\tB" -> tab snaps from 1 to 4, B at col 5
TEST(CursorTest, OneCharThenTabThenChar) {
    Cursor cursor;
    cursor.advance('A');
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 4);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 5);
}

// "AAA\tB" -> tab snaps from 3 to 4, B at col 5
TEST(CursorTest, ThreeCharsThenTabThenChar) {
    Cursor cursor;
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 4);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 5);
}

// "AAAA\tB" -> tab snaps from 4 to 8, B at col 9
TEST(CursorTest, FourCharsThenTabThenChar) {
    Cursor cursor;
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 8);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 9);
}

// "\tAA\tB" -> first tab to 4, AA at 5-6, second tab snaps to 8, B at 9
TEST(CursorTest, TabCharCharTabChar) {
    Cursor cursor;
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 4);
    cursor.advance('A');
    cursor.advance('A');
    EXPECT_EQ(cursor.column(), 6);
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 8);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 9);
}

// "\t\tB" -> double tab: 0->4->8, B at 9
TEST(CursorTest, DoubleTabThenChar) {
    Cursor cursor;
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 4);
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 8);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 9);
}

// "AAAAA\tB" -> 5 chars, tab snaps from 5 to 8, B at 9
TEST(CursorTest, FiveCharsThenTabThenChar) {
    Cursor cursor;
    for (int i = 0; i < 5; i++) {
        cursor.advance('A');
    }
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 8);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 9);
}

// "AA\tAA\tB" -> tab snaps 2->4, AA at 5-6, tab snaps 6->8, B at 9
TEST(CursorTest, MixedCharsAndTabs) {
    Cursor cursor;
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 4);
    cursor.advance('A');
    cursor.advance('A');
    cursor.advance('\t');
    EXPECT_EQ(cursor.column(), 8);
    cursor.advance('B');
    EXPECT_EQ(cursor.column(), 9);
}
