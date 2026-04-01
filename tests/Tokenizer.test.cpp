#include <gtest/gtest.h>

#include <CharacterAttributes.hpp>
#include <StringSource.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>

class TokenizerTest : public ::testing::Test {
protected:
    SymbolStore symbols;
    CharacterAttributes attributes;

    void expectToken(
        const Tokenizer& tokenizer,
        size_t index,
        const std::string& value,
        SymbolType type,
        size_t row,
        size_t col
    ) {
        ASSERT_LT(index, tokenizer.tokens().size())
            << "Token index " << index << " out of range";
        const auto& token = tokenizer.tokens()[index];
        EXPECT_EQ(symbols.lookup(token.code), value)
            << "Token " << index << " value mismatch";
        EXPECT_EQ(symbols.lookupType(token.code), type)
            << "Token " << index << " type mismatch";
        EXPECT_EQ(token.row, row) << "Token " << index << " row mismatch";
        EXPECT_EQ(token.column, col) << "Token " << index << " col mismatch";
    }

    void expectError(
        const Tokenizer& tokenizer,
        size_t index,
        size_t row,
        size_t col
    ) {
        ASSERT_LT(index, tokenizer.errors().size())
            << "Error index " << index << " out of range";
        const auto& error = tokenizer.errors()[index];
        EXPECT_EQ(error.row, row) << "Error " << index << " row mismatch";
        EXPECT_EQ(error.column, col) << "Error " << index << " col mismatch";
    }
};

// --- No tokens ---

TEST_F(TokenizerTest, YieldsNoTokensForEmptySource) {
    StringSource source("");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForWhitespaceOnly) {
    StringSource source(" \t\n\r\v \f\t\r \n\t ");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForEmptyComment) {
    StringSource source("(**)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForOneLineComment) {
    StringSource source("(* comment *)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForMultilineComment) {
    StringSource source("(* l1 \n l2 *)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForMultipleComments) {
    StringSource source("(* comment *) (* l1 \n l2 *) \n (* comment *)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForCommentsWithStars) {
    StringSource source("(*****)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForCommentsWithDelimiters) {
    StringSource source("(*;.:*)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForCommentWithNestedParensAndStars) {
    StringSource source("(*(()*())*)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

TEST_F(TokenizerTest, YieldsNoTokensForSpaceOnlyComment) {
    StringSource source("(* *)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

// --- Single tokens ---

TEST_F(TokenizerTest, YieldsCorrectTokenForKeyword) {
    // "PROGRAM"
    StringSource source("PROGRAM");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);

    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
}

TEST_F(TokenizerTest, YieldsCorrectTokenForIdentifier) {
    StringSource source("TEST");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "TEST", SymbolType::Identifier, 0, 0);
}

TEST_F(TokenizerTest, YieldsCorrectTokenForSingleLetter) {
    StringSource source("A");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "A", SymbolType::Identifier, 0, 0);
}

TEST_F(TokenizerTest, YieldsCorrectTokenForLiteral) {
    StringSource source("10");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "10", SymbolType::Literal, 0, 0);
}

TEST_F(TokenizerTest, YieldsCorrectTokenForDelimiter) {
    StringSource source(";");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, ";", SymbolType::Ascii, 0, 0);
}

TEST_F(TokenizerTest, YieldsCorrectTokenForOpenParenNotComment) {
    StringSource source("(");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "(", SymbolType::Ascii, 0, 0);
}

// --- Multi-token sequences ---

TEST_F(TokenizerTest, YieldsCorrectTokensForProgramTest) {
    StringSource source("PROGRAM TEST;");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 3);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
    expectToken(tokenizer, 1, "TEST", SymbolType::Identifier, 0, 8);
    expectToken(tokenizer, 2, ";", SymbolType::Ascii, 0, 12);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForConstExpression) {
    StringSource source("X = '10$EXP(20)';");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 10);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "X", SymbolType::Identifier, 0, 0);
    expectToken(tokenizer, 1, "=", SymbolType::Ascii, 0, 2);
    expectToken(tokenizer, 2, "'", SymbolType::Ascii, 0, 4);
    expectToken(tokenizer, 3, "10", SymbolType::Literal, 0, 5);
    expectToken(tokenizer, 4, "$EXP", SymbolType::MultiDelimiter, 0, 7);
    expectToken(tokenizer, 5, "(", SymbolType::Ascii, 0, 11);
    expectToken(tokenizer, 6, "20", SymbolType::Literal, 0, 12);
    expectToken(tokenizer, 7, ")", SymbolType::Ascii, 0, 14);
    expectToken(tokenizer, 8, "'", SymbolType::Ascii, 0, 15);
    expectToken(tokenizer, 9, ";", SymbolType::Ascii, 0, 16);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForCompactAssignment) {
    StringSource source("Y='3,4';");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 8);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "Y", SymbolType::Identifier, 0, 0);
    expectToken(tokenizer, 1, "=", SymbolType::Ascii, 0, 1);
    expectToken(tokenizer, 2, "'", SymbolType::Ascii, 0, 2);
    expectToken(tokenizer, 3, "3", SymbolType::Literal, 0, 3);
    expectToken(tokenizer, 4, ",", SymbolType::Ascii, 0, 4);
    expectToken(tokenizer, 5, "4", SymbolType::Literal, 0, 5);
    expectToken(tokenizer, 6, "'", SymbolType::Ascii, 0, 6);
    expectToken(tokenizer, 7, ";", SymbolType::Ascii, 0, 7);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForBlockWithComment) {
    StringSource source("BEGIN\n(* nothing *)\nEND.");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 3);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "BEGIN", SymbolType::Keyword, 0, 0);
    expectToken(tokenizer, 1, "END", SymbolType::Keyword, 2, 0);
    expectToken(tokenizer, 2, ".", SymbolType::Ascii, 2, 3);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForDelimitersOnly) {
    StringSource source("...;;,(()()());;,=,=,,,=;;,.");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 28);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, ".", SymbolType::Ascii, 0, 0);
    expectToken(tokenizer, 27, ".", SymbolType::Ascii, 0, 27);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForLab1Example) {
    StringSource source(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  (* This is a comment. *)\n"
        "  X = '10$EXP(20)';\n"
        "  Y = '30,40';\n"
        "BEGIN\n"
        "END."
    );
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.errors().size(), 0);

    // Row 0: PROGRAM TEST;
    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
    expectToken(tokenizer, 1, "TEST", SymbolType::Identifier, 0, 8);
    expectToken(tokenizer, 2, ";", SymbolType::Ascii, 0, 12);

    // Row 1: CONST
    expectToken(tokenizer, 3, "CONST", SymbolType::Keyword, 1, 0);

    // Row 2: comment — no tokens

    // Row 3: X = '10$EXP(20)';
    expectToken(tokenizer, 4, "X", SymbolType::Identifier, 3, 2);
    expectToken(tokenizer, 5, "=", SymbolType::Ascii, 3, 4);
    expectToken(tokenizer, 6, "'", SymbolType::Ascii, 3, 6);
    expectToken(tokenizer, 7, "10", SymbolType::Literal, 3, 7);
    expectToken(tokenizer, 8, "$EXP", SymbolType::MultiDelimiter, 3, 9);
    expectToken(tokenizer, 9, "(", SymbolType::Ascii, 3, 13);
    expectToken(tokenizer, 10, "20", SymbolType::Literal, 3, 14);
    expectToken(tokenizer, 11, ")", SymbolType::Ascii, 3, 16);
    expectToken(tokenizer, 12, "'", SymbolType::Ascii, 3, 17);
    expectToken(tokenizer, 13, ";", SymbolType::Ascii, 3, 18);

    // Row 4: Y = '30,40';
    expectToken(tokenizer, 14, "Y", SymbolType::Identifier, 4, 2);
    expectToken(tokenizer, 15, "=", SymbolType::Ascii, 4, 4);
    expectToken(tokenizer, 16, "'", SymbolType::Ascii, 4, 6);
    expectToken(tokenizer, 17, "30", SymbolType::Literal, 4, 7);
    expectToken(tokenizer, 18, ",", SymbolType::Ascii, 4, 9);
    expectToken(tokenizer, 19, "40", SymbolType::Literal, 4, 10);
    expectToken(tokenizer, 20, "'", SymbolType::Ascii, 4, 12);
    expectToken(tokenizer, 21, ";", SymbolType::Ascii, 4, 13);

    // Row 5: BEGIN
    expectToken(tokenizer, 22, "BEGIN", SymbolType::Keyword, 5, 0);

    // Row 6: END.
    expectToken(tokenizer, 23, "END", SymbolType::Keyword, 6, 0);
    expectToken(tokenizer, 24, ".", SymbolType::Ascii, 6, 3);

    EXPECT_EQ(tokenizer.tokens().size(), 25);
}

// --- Adjacent token boundary ---

TEST_F(TokenizerTest, YieldsCorrectTokensForAdjacentDigitsAndLetters) {
    StringSource source("10X");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 2);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "10", SymbolType::Literal, 0, 0);
    expectToken(tokenizer, 1, "X", SymbolType::Identifier, 0, 2);
}

// --- Edge case: "(" at end of input ---

TEST_F(TokenizerTest, YieldsCorrectTokenForOpenParenAtEndOfInput) {
    StringSource source("(");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    EXPECT_EQ(tokenizer.errors().size(), 0);
    expectToken(tokenizer, 0, "(", SymbolType::Ascii, 0, 0);
}

// --- Error tests ---

TEST_F(TokenizerTest, YieldsErrorForUnknownSymbol) {
    StringSource source("&");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
}

TEST_F(TokenizerTest, YieldsErrorsForMultipleUnknownSymbols) {
    StringSource source("&^%");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 3);
    expectError(tokenizer, 0, 0, 0);
    expectError(tokenizer, 1, 0, 1);
    expectError(tokenizer, 2, 0, 2);
}

TEST_F(TokenizerTest, YieldsErrorsForUnknownSymbolsWithSpaces) {
    StringSource source("& ^ %");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 3);
    expectError(tokenizer, 0, 0, 0);
    expectError(tokenizer, 1, 0, 2);
    expectError(tokenizer, 2, 0, 4);
}

TEST_F(TokenizerTest, YieldsErrorForUnknownSymbolInsideKeyword) {
    StringSource source("BEGI@");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 1);
    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectToken(tokenizer, 0, "BEGI", SymbolType::Identifier, 0, 0);
    expectError(tokenizer, 0, 0, 4);
}

TEST_F(TokenizerTest, YieldsErrorsForLowercaseLetters) {
    StringSource source("test");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 4);
    expectError(tokenizer, 0, 0, 0);
    expectError(tokenizer, 1, 0, 1);
    expectError(tokenizer, 2, 0, 2);
    expectError(tokenizer, 3, 0, 3);
}

TEST_F(TokenizerTest, YieldsErrorsInterleavedWithValidTokens) {
    StringSource source("PROGRAM & TEST ^ ;");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.tokens().size(), 3);
    ASSERT_EQ(tokenizer.errors().size(), 2);
    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
    expectError(tokenizer, 0, 0, 8);
    expectToken(tokenizer, 1, "TEST", SymbolType::Identifier, 0, 10);
    expectError(tokenizer, 1, 0, 15);
    expectToken(tokenizer, 2, ";", SymbolType::Ascii, 0, 17);
}

// --- Unclosed comment errors ---

TEST_F(TokenizerTest, YieldsErrorForUnclosedComment) {
    StringSource source("(* comment");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
}

TEST_F(TokenizerTest, YieldsErrorForUnclosedCommentWithStar) {
    StringSource source("(* comment *");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
}

TEST_F(TokenizerTest, YieldsErrorForMinimalUnclosedComment) {
    StringSource source("(*");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
}

// --- Comment with forbidden characters ---

TEST_F(TokenizerTest, YieldsNoErrorForCommentWithForbiddenCharacters) {
    StringSource source("(* !@#$%^& *)");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    EXPECT_EQ(tokenizer.errors().size(), 0);
}

// --- Multi-line program tests ---

TEST_F(TokenizerTest, YieldsCorrectTokensForSimpleProgram) {
    StringSource source(
        "PROGRAM HELLO;\n"
        "CONST\n"
        "  A = '5';\n"
        "  B = '10';\n"
        "BEGIN\n"
        "END."
    );
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.errors().size(), 0);

    // Row 0: PROGRAM HELLO;
    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
    expectToken(tokenizer, 1, "HELLO", SymbolType::Identifier, 0, 8);
    expectToken(tokenizer, 2, ";", SymbolType::Ascii, 0, 13);

    // Row 1: CONST
    expectToken(tokenizer, 3, "CONST", SymbolType::Keyword, 1, 0);

    // Row 2:   A = '5';
    expectToken(tokenizer, 4, "A", SymbolType::Identifier, 2, 2);
    expectToken(tokenizer, 5, "=", SymbolType::Ascii, 2, 4);
    expectToken(tokenizer, 6, "'", SymbolType::Ascii, 2, 6);
    expectToken(tokenizer, 7, "5", SymbolType::Literal, 2, 7);
    expectToken(tokenizer, 8, "'", SymbolType::Ascii, 2, 8);
    expectToken(tokenizer, 9, ";", SymbolType::Ascii, 2, 9);

    // Row 3:   B = '10';
    expectToken(tokenizer, 10, "B", SymbolType::Identifier, 3, 2);
    expectToken(tokenizer, 11, "=", SymbolType::Ascii, 3, 4);
    expectToken(tokenizer, 12, "'", SymbolType::Ascii, 3, 6);
    expectToken(tokenizer, 13, "10", SymbolType::Literal, 3, 7);
    expectToken(tokenizer, 14, "'", SymbolType::Ascii, 3, 9);
    expectToken(tokenizer, 15, ";", SymbolType::Ascii, 3, 10);

    // Row 4: BEGIN
    expectToken(tokenizer, 16, "BEGIN", SymbolType::Keyword, 4, 0);

    // Row 5: END.
    expectToken(tokenizer, 17, "END", SymbolType::Keyword, 5, 0);
    expectToken(tokenizer, 18, ".", SymbolType::Ascii, 5, 3);

    EXPECT_EQ(tokenizer.tokens().size(), 19);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForProgramWithCommentsAndExpressions) {
    StringSource source(
        "PROGRAM CALC;\n"
        "CONST\n"
        "  (* Initialize constants *)\n"
        "  X = '100$EXP(2)';\n"
        "  Y = '50,25';\n"
        "  (* multi\n"
        "  line *)\n"
        "  Z = '0';\n"
        "BEGIN\n"
        "END."
    );
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.errors().size(), 0);

    // Row 0: PROGRAM CALC;
    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
    expectToken(tokenizer, 1, "CALC", SymbolType::Identifier, 0, 8);
    expectToken(tokenizer, 2, ";", SymbolType::Ascii, 0, 12);

    // Row 1: CONST
    expectToken(tokenizer, 3, "CONST", SymbolType::Keyword, 1, 0);

    // Row 2: comment — no tokens

    // Row 3:   X = '100$EXP(2)';
    expectToken(tokenizer, 4, "X", SymbolType::Identifier, 3, 2);
    expectToken(tokenizer, 5, "=", SymbolType::Ascii, 3, 4);
    expectToken(tokenizer, 6, "'", SymbolType::Ascii, 3, 6);
    expectToken(tokenizer, 7, "100", SymbolType::Literal, 3, 7);
    expectToken(tokenizer, 8, "$EXP", SymbolType::MultiDelimiter, 3, 10);
    expectToken(tokenizer, 9, "(", SymbolType::Ascii, 3, 14);
    expectToken(tokenizer, 10, "2", SymbolType::Literal, 3, 15);
    expectToken(tokenizer, 11, ")", SymbolType::Ascii, 3, 16);
    expectToken(tokenizer, 12, "'", SymbolType::Ascii, 3, 17);
    expectToken(tokenizer, 13, ";", SymbolType::Ascii, 3, 18);

    // Row 4:   Y = '50,25';
    expectToken(tokenizer, 14, "Y", SymbolType::Identifier, 4, 2);
    expectToken(tokenizer, 15, "=", SymbolType::Ascii, 4, 4);
    expectToken(tokenizer, 16, "'", SymbolType::Ascii, 4, 6);
    expectToken(tokenizer, 17, "50", SymbolType::Literal, 4, 7);
    expectToken(tokenizer, 18, ",", SymbolType::Ascii, 4, 9);
    expectToken(tokenizer, 19, "25", SymbolType::Literal, 4, 10);
    expectToken(tokenizer, 20, "'", SymbolType::Ascii, 4, 12);
    expectToken(tokenizer, 21, ";", SymbolType::Ascii, 4, 13);

    // Row 5-6: multiline comment — no tokens

    // Row 7:   Z = '0';
    expectToken(tokenizer, 22, "Z", SymbolType::Identifier, 7, 2);
    expectToken(tokenizer, 23, "=", SymbolType::Ascii, 7, 4);
    expectToken(tokenizer, 24, "'", SymbolType::Ascii, 7, 6);
    expectToken(tokenizer, 25, "0", SymbolType::Literal, 7, 7);
    expectToken(tokenizer, 26, "'", SymbolType::Ascii, 7, 8);
    expectToken(tokenizer, 27, ";", SymbolType::Ascii, 7, 9);

    // Row 8: BEGIN
    expectToken(tokenizer, 28, "BEGIN", SymbolType::Keyword, 8, 0);

    // Row 9: END.
    expectToken(tokenizer, 29, "END", SymbolType::Keyword, 9, 0);
    expectToken(tokenizer, 30, ".", SymbolType::Ascii, 9, 3);

    EXPECT_EQ(tokenizer.tokens().size(), 31);
}

TEST_F(TokenizerTest, YieldsCorrectTokensForProgramWithEdgeCaseComments) {
    StringSource source(
        "PROGRAM EDGE;\n"
        "CONST\n"
        "  (**) (* nested (* stars *** *)\n"
        "  A = '1$EXP(0)';\n"
        "BEGIN\n"
        "END."
    );
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.errors().size(), 0);

    // Row 0: PROGRAM EDGE;
    expectToken(tokenizer, 0, "PROGRAM", SymbolType::Keyword, 0, 0);
    expectToken(tokenizer, 1, "EDGE", SymbolType::Identifier, 0, 8);
    expectToken(tokenizer, 2, ";", SymbolType::Ascii, 0, 12);

    // Row 1: CONST
    expectToken(tokenizer, 3, "CONST", SymbolType::Keyword, 1, 0);

    // Row 2: (**) and (* nested (* stars *** *) — both are comments, no tokens

    // Row 3:   A = '1$EXP(0)';
    expectToken(tokenizer, 4, "A", SymbolType::Identifier, 3, 2);
    expectToken(tokenizer, 5, "=", SymbolType::Ascii, 3, 4);
    expectToken(tokenizer, 6, "'", SymbolType::Ascii, 3, 6);
    expectToken(tokenizer, 7, "1", SymbolType::Literal, 3, 7);
    expectToken(tokenizer, 8, "$EXP", SymbolType::MultiDelimiter, 3, 8);
    expectToken(tokenizer, 9, "(", SymbolType::Ascii, 3, 12);
    expectToken(tokenizer, 10, "0", SymbolType::Literal, 3, 13);
    expectToken(tokenizer, 11, ")", SymbolType::Ascii, 3, 14);
    expectToken(tokenizer, 12, "'", SymbolType::Ascii, 3, 15);
    expectToken(tokenizer, 13, ";", SymbolType::Ascii, 3, 16);

    // Row 4: BEGIN
    expectToken(tokenizer, 14, "BEGIN", SymbolType::Keyword, 4, 0);

    // Row 5: END.
    expectToken(tokenizer, 15, "END", SymbolType::Keyword, 5, 0);
    expectToken(tokenizer, 16, ".", SymbolType::Ascii, 5, 3);

    EXPECT_EQ(tokenizer.tokens().size(), 17);
}

// --- Invalid $ usage ---

TEST_F(TokenizerTest, YieldsErrorForDollarAtEndOfInput) {
    StringSource source("$");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    EXPECT_EQ(tokenizer.tokens().size(), 0);
    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
}

TEST_F(TokenizerTest, YieldsErrorForDollarFollowedBySpace) {
    StringSource source("$ EXP");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
    // EXP is scanned as a separate identifier
    ASSERT_EQ(tokenizer.tokens().size(), 1);
    expectToken(tokenizer, 0, "EXP", SymbolType::Identifier, 0, 2);
}

TEST_F(TokenizerTest, YieldsErrorForDollarFollowedByDigits) {
    StringSource source("$123");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
}

TEST_F(TokenizerTest, YieldsErrorForDollarFollowedByWrongLetters) {
    StringSource source("$EXON");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 0);
    EXPECT_EQ(tokenizer.tokens().size(), 0);
}

TEST_F(TokenizerTest, YieldsErrorForDollarFollowedByNewline) {
    StringSource source("ABCD\t$\nEXP");
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 0, 8);  // $ at col 8
    // ABCD and EXP are still scanned
    ASSERT_EQ(tokenizer.tokens().size(), 2);
    expectToken(tokenizer, 0, "ABCD", SymbolType::Identifier, 0, 0);
    expectToken(tokenizer, 1, "EXP", SymbolType::Identifier, 1, 0);
}

// --- Multi-line error test ---

TEST_F(TokenizerTest, YieldsCorrectErrorsInMultilineProgram) {
    StringSource source(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '10';\n"
        "  B = '20';\n"
        "  X & Y;\n"
        "BEGIN\n"
        "END."
    );
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    ASSERT_EQ(tokenizer.errors().size(), 1);
    expectError(tokenizer, 0, 4, 4);  // '&' at row 4, col 4

    // Verify tokens around the error are still correct
    EXPECT_EQ(tokenizer.tokens().size(), 22);
}
