#include <gtest/gtest.h>
#include <fstream>
#include <cstdlib>
#include <unistd.h>

#include <FileSource.class.hpp>

class FileSourceTest : public ::testing::Test {
protected:
    std::string tempPath;

    void createTempFile(const std::string& content) {
        char tmpl[] = "/tmp/source_test_XXXXXX";
        int fd = mkstemp(tmpl);
        tempPath = tmpl;
        close(fd);
        std::ofstream out(tempPath);
        out << content;
    }

    void TearDown() override {
        if (!tempPath.empty()) {
            std::remove(tempPath.c_str());
        }
    }
};

TEST_F(FileSourceTest, PointsToFirstCharAfterInit) {
    createTempFile("ABC");
    FileSource source(tempPath);
    EXPECT_EQ(source.current(), 'A');
}

TEST_F(FileSourceTest, PointsToCorrectCharAfterOneRead) {
    createTempFile("ABC");
    FileSource source(tempPath);
    char next = source.read();
    EXPECT_EQ(next, 'B');
    EXPECT_EQ(source.current(), 'B');
}

TEST_F(FileSourceTest, PointsToCorrectCharAfterMultipleReads) {
    createTempFile("ABCDE");
    FileSource source(tempPath);
    source.read();
    source.read();
    char c = source.read();
    EXPECT_EQ(c, 'D');
    EXPECT_EQ(source.current(), 'D');
}

TEST_F(FileSourceTest, DoneIsFalseWhenCharsRemain) {
    createTempFile("AB");
    FileSource source(tempPath);
    EXPECT_FALSE(source.done());
    source.read();
    EXPECT_FALSE(source.done());
}

TEST_F(FileSourceTest, DoneIsTrueAtEnd) {
    createTempFile("AB");
    FileSource source(tempPath);
    source.read();
    source.read();
    EXPECT_TRUE(source.done());
}

TEST_F(FileSourceTest, DoneIsTrueForEmptyFile) {
    createTempFile("");
    FileSource source(tempPath);
    EXPECT_TRUE(source.done());
}

TEST_F(FileSourceTest, CurrentReturnsCorrectChar) {
    createTempFile("XYZ");
    FileSource source(tempPath);
    EXPECT_EQ(source.current(), 'X');
    source.read();
    EXPECT_EQ(source.current(), 'Y');
    source.read();
    EXPECT_EQ(source.current(), 'Z');
}

TEST_F(FileSourceTest, TracksColumn) {
    createTempFile("ABC");
    FileSource source(tempPath);
    EXPECT_EQ(source.column(), 0);
    source.read();
    EXPECT_EQ(source.column(), 1);
    source.read();
    EXPECT_EQ(source.column(), 2);
}

TEST_F(FileSourceTest, TracksRow) {
    createTempFile("A\nB\nC");
    FileSource source(tempPath);
    EXPECT_EQ(source.row(), 0);
    source.read(); // '\n'
    source.read(); // 'B'
    EXPECT_EQ(source.row(), 1);
    source.read(); // '\n'
    source.read(); // 'C'
    EXPECT_EQ(source.row(), 2);
}

TEST_F(FileSourceTest, SingleCharInput) {
    createTempFile("X");
    FileSource source(tempPath);
    EXPECT_FALSE(source.done());
    EXPECT_EQ(source.current(), 'X');
    source.read();
    EXPECT_TRUE(source.done());
}
