#include <gtest/gtest.h>

#include <AssemblyFormatter.hpp>

// --- Common ASM column widths: label=8, instruction=8, operands=16 ---

class CommonAsmWidthFormatterTest : public ::testing::Test {
protected:
    AssemblyFormatter formatter{{
        .labelWidth = 8,
        .instructionWidth = 8,
        .operandsWidth = 16,
    }};
};

TEST_F(CommonAsmWidthFormatterTest, BlankLineIsJustNewline) {
    EXPECT_EQ(formatter.blankLine(), "\n");
}

TEST_F(CommonAsmWidthFormatterTest, CommentLineHasSemicolonPrefix) {
    EXPECT_EQ(formatter.commentLine("hello"), "; hello\n");
}

TEST_F(CommonAsmWidthFormatterTest, LabelLineAppendsColonAndNoIndent) {
    EXPECT_EQ(formatter.labelLine("_start"), "_start:\n");
}

TEST_F(CommonAsmWidthFormatterTest, SectionLineIndentedToLabelWidth) {
    EXPECT_EQ(formatter.sectionLine(".text"), "        section .text:\n");
}

TEST_F(CommonAsmWidthFormatterTest, BareInstructionLineIndentedToLabelWidth) {
    EXPECT_EQ(formatter.instructionLine("nop"), "        nop\n");
}

TEST_F(
    CommonAsmWidthFormatterTest,
    InstructionWithOperandsPadsInstructionField
) {
    // 8-space label indent, "mov" padded to 8-char field, then operands.
    EXPECT_EQ(
        formatter.instructionLine("mov", "eax, 60"), "        mov     eax, 60\n"
    );
}

TEST_F(CommonAsmWidthFormatterTest, ConstantLinePadsLabelAndSizeFields) {
    // "X:" padded to 8-char field, "dd" padded to 8-char field, then value.
    EXPECT_EQ(
        formatter.constantLine("X", "dd", "1, 0"), "X:      dd      1, 0\n"
    );
}

TEST_F(
    CommonAsmWidthFormatterTest,
    OverlongInstructionStillFollowedBySingleSpace
) {
    // Instruction longer than its field shouldn't get truncated; one separator.
    EXPECT_EQ(
        formatter.instructionLine("verylonginstr", "x"),
        "        verylonginstr x\n"
    );
}

// --- All-zero widths: no padding, minimal whitespace ---

class ZeroWidthFormatterTest : public ::testing::Test {
protected:
    AssemblyFormatter formatter{{}};
};

TEST_F(ZeroWidthFormatterTest, BlankLineIsJustNewline) {
    EXPECT_EQ(formatter.blankLine(), "\n");
}

TEST_F(ZeroWidthFormatterTest, CommentLineHasSemicolonPrefix) {
    EXPECT_EQ(formatter.commentLine("hello"), "; hello\n");
}

TEST_F(ZeroWidthFormatterTest, LabelLineAppendsColonAndNoIndent) {
    EXPECT_EQ(formatter.labelLine("_start"), "_start:\n");
}

TEST_F(ZeroWidthFormatterTest, SectionLineHasNoIndent) {
    EXPECT_EQ(formatter.sectionLine(".text"), "section .text:\n");
}

TEST_F(ZeroWidthFormatterTest, BareInstructionLineHasNoIndent) {
    EXPECT_EQ(formatter.instructionLine("nop"), "nop\n");
}

TEST_F(ZeroWidthFormatterTest, InstructionWithOperandsHasSingleSpaceSeparator) {
    EXPECT_EQ(formatter.instructionLine("mov", "eax, 60"), "mov eax, 60\n");
}

TEST_F(ZeroWidthFormatterTest, ConstantLineHasSingleSpaceSeparators) {
    EXPECT_EQ(formatter.constantLine("X", "dd", "1, 0"), "X: dd 1, 0\n");
}
