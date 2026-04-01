#include "FileSource.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

class FileSourceTest : public ::testing::Test {
protected:
    std::filesystem::path tempPath;

    void createTempFile(const std::string& content) {
        tempPath = std::filesystem::temp_directory_path() / "source_test.tmp";
        std::ofstream out(tempPath);
        out << content;
    }

    void TearDown() override {
        if (!tempPath.empty()) {
            std::filesystem::remove(tempPath);
        }
    }
};

TEST_F(FileSourceTest, PointsToFirstCharAfterInit) {
    createTempFile("ABC");
    FileSource source(tempPath.string());
    EXPECT_EQ(source.current(), 'A');
}

TEST_F(FileSourceTest, PointsToCorrectCharAfterOneRead) {
    createTempFile("ABC");
    FileSource source(tempPath.string());
    char next = source.read();
    EXPECT_EQ(next, 'B');
    EXPECT_EQ(source.current(), 'B');
}

TEST_F(FileSourceTest, PointsToCorrectCharAfterMultipleReads) {
    createTempFile("ABCDE");
    FileSource source(tempPath.string());
    source.read();
    source.read();
    char c = source.read();
    EXPECT_EQ(c, 'D');
    EXPECT_EQ(source.current(), 'D');
}

TEST_F(FileSourceTest, DoneIsFalseWhenCharsRemain) {
    createTempFile("AB");
    FileSource source(tempPath.string());
    EXPECT_FALSE(source.done());
    source.read();
    EXPECT_FALSE(source.done());
}

TEST_F(FileSourceTest, DoneIsTrueAtEnd) {
    createTempFile("AB");
    FileSource source(tempPath.string());
    source.read();
    source.read();
    EXPECT_TRUE(source.done());
}

TEST_F(FileSourceTest, DoneIsTrueForEmptyFile) {
    createTempFile("");
    FileSource source(tempPath.string());
    EXPECT_TRUE(source.done());
}

TEST_F(FileSourceTest, CurrentReturnsCorrectChar) {
    createTempFile("XYZ");
    FileSource source(tempPath.string());
    EXPECT_EQ(source.current(), 'X');
    source.read();
    EXPECT_EQ(source.current(), 'Y');
    source.read();
    EXPECT_EQ(source.current(), 'Z');
}

TEST_F(FileSourceTest, TracksColumn) {
    createTempFile("ABC");
    FileSource source(tempPath.string());
    EXPECT_EQ(source.cursor().column(), 0);
    source.read();
    EXPECT_EQ(source.cursor().column(), 1);
    source.read();
    EXPECT_EQ(source.cursor().column(), 2);
}

TEST_F(FileSourceTest, TracksRow) {
    createTempFile("A\nB\nC");
    FileSource source(tempPath.string());
    EXPECT_EQ(source.cursor().row(), 0);
    source.read();  // '\n'
    source.read();  // 'B'
    EXPECT_EQ(source.cursor().row(), 1);
    source.read();  // '\n'
    source.read();  // 'C'
    EXPECT_EQ(source.cursor().row(), 2);
}

TEST_F(FileSourceTest, SingleCharInput) {
    createTempFile("X");
    FileSource source(tempPath.string());
    EXPECT_FALSE(source.done());
    EXPECT_EQ(source.current(), 'X');
    source.read();
    EXPECT_TRUE(source.done());
}
