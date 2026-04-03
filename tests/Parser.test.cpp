#include <gtest/gtest.h>

#include <CharacterAttributes.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <StringSource.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>
#include <format>

class ParserTest : public ::testing::Test {
protected:
    SymbolStore symbols;
    CharacterAttributes attributes;
    std::ostringstream logOutput;
    Logger<Error> logger{
        "Tokenizer",
        [](const Error& err) {
            return std::format(
                "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
            );
        },
        logOutput
    };

    void SetUp() override {
        logger.clear();
        logOutput.str("");
    }

    void expectError(size_t index, const std::string& message) {
        ASSERT_LT(index, logger.messages().size())
            << "Error index " << index << " out of range";
        EXPECT_EQ(logger.messages()[index].message, message)
            << "Error " << index << " message mismatch";
    }

    Parser makeParser(const std::string& input) {
        StringSource source(input);
        Tokenizer tokenizer(source, symbols, attributes, logger);
        tokenizer.scan();
        return Parser(symbols, tokenizer.tokens(), logger);
    }
};

// --- Empty program (no constants) ---

TEST_F(ParserTest, ParsesEmptyProgram) {
    auto parser = makeParser("PROGRAM EMPTY; BEGIN END.");
    parser.parse();

    auto& root = parser.tree().root();
    EXPECT_EQ(root.data().symbol, "<signal-program>");
    EXPECT_EQ(root.data().rule, "S");

    // Rule 1: <signal-program> --> <program>
    auto& program = *root.children()[0];
    EXPECT_EQ(program.data().rule, "1");
    ASSERT_EQ(program.children().size(), 1);

    // Rule 2: PROGRAM <procedure-identifier>; <block>.
    // 2 nonterminals: <procedure-identifier>, <block>
    auto& programBody = *program.children()[0];
    EXPECT_EQ(programBody.data().rule, "2");
    ASSERT_EQ(programBody.children().size(), 2);

    // Rule 3: <declarations> BEGIN <statements-list> END
    // 2 nonterminals: <declarations>, <statements-list>
    auto& block = *programBody.children()[1];
    EXPECT_EQ(block.data().rule, "3");
    ASSERT_EQ(block.children().size(), 2);

    // Rule 5: <declarations> --> <constant-declarations> — 1 nonterminal
    auto& declarations = *block.children()[0];
    EXPECT_EQ(declarations.data().rule, "5");
    ASSERT_EQ(declarations.children().size(), 1);

    // Rule 6: <constant-declarations> --> <empty> — 0 nonterminals
    auto& constDecls = *declarations.children()[0];
    EXPECT_EQ(constDecls.data().symbol, "<empty>");
    EXPECT_EQ(constDecls.data().rule, "6");

    // Rule 4: <statements-list> --> <empty> — 0 nonterminals
    auto& stmtList = *block.children()[1];
    EXPECT_EQ(stmtList.data().symbol, "<empty>");
    EXPECT_EQ(stmtList.data().rule, "4");
}

// --- Basic program with one constant ---

TEST_F(ParserTest, ParsesProgramWithOneConstant) {
    auto parser = makeParser(
        "PROGRAM HELLO;\n"
        "CONST\n"
        "  X = '42';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& declarations = *block.children()[0];

    // Rule 6: CONST <constant-declarations-list> — 1 nonterminal
    auto& constDecls = *declarations.children()[0];
    EXPECT_EQ(constDecls.data().symbol, "CONST <constant-declarations-list>");
    EXPECT_EQ(constDecls.data().rule, "6");
    ASSERT_EQ(constDecls.children().size(), 1);

    // Rule 7: <constant-declaration> <constant-declarations-list> — 2
    // nonterminals
    auto& declList = *constDecls.children()[0];
    EXPECT_EQ(declList.data().rule, "7");
    ASSERT_EQ(declList.children().size(), 2);

    // Rule 8: <constant-identifier> = <constant>; — 2 nonterminals
    auto& decl = *declList.children()[0];
    EXPECT_EQ(decl.data().rule, "8");
    ASSERT_EQ(decl.children().size(), 2);

    // Rule 13 -> Rule 15: identifier = X
    auto& constId = *decl.children()[0];
    EXPECT_EQ(constId.data().rule, "13");
    auto& ident = *constId.children()[0];
    EXPECT_EQ(ident.data().symbol, "X");

    // Rule 9: '<complex-number>' — 1 nonterminal
    auto& constant = *decl.children()[1];
    EXPECT_EQ(constant.data().rule, "9");
    ASSERT_EQ(constant.children().size(), 1);

    // Rule 10: <left-part> <right-part> — 2 nonterminals
    auto& complexNum = *constant.children()[0];
    EXPECT_EQ(complexNum.data().rule, "10");
    ASSERT_EQ(complexNum.children().size(), 2);

    // Rule 11: <left-part> --> <unsigned-integer>
    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().symbol, "<unsigned-integer>");

    // Rule 12: <right-part> --> <empty>
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().symbol, "<empty>");

    // Rule 7: list ends with <empty>
    auto& declListEnd = *declList.children()[1];
    EXPECT_EQ(declListEnd.data().symbol, "<empty>");
    EXPECT_EQ(declListEnd.data().rule, "7");
}

// --- Constant with comma variant ---

TEST_F(ParserTest, ParsesConstantWithCommaVariant) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '2,3';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    // declarations[0] -> constDecls[0] -> declList[0] -> decl
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    // decl[1] = constant, constant[0] = complexNum
    auto& complexNum = *decl.children()[1]->children()[0];

    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().symbol, "<unsigned-integer>");
    EXPECT_EQ(leftPart.data().rule, "11");

    // Rule 12: ,<unsigned-integer> — 1 nonterminal
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().symbol, ",<unsigned-integer>");
    EXPECT_EQ(rightPart.data().rule, "12");
    ASSERT_EQ(rightPart.children().size(), 1);
}

// --- Constant with $EXP variant ---

TEST_F(ParserTest, ParsesConstantWithExpVariant) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '5$EXP(6)';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().symbol, "<unsigned-integer>");

    // Rule 12: $EXP( <unsigned-integer> ) — 1 nonterminal
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().symbol, "$EXP( <unsigned-integer> )");
    EXPECT_EQ(rightPart.data().rule, "12");
    ASSERT_EQ(rightPart.children().size(), 1);
}

// --- Empty complex number '' ---

TEST_F(ParserTest, ParsesEmptyComplexNumber) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().symbol, "<empty>");
    EXPECT_EQ(complexNum.children()[1]->data().symbol, "<empty>");
}

// --- Multiple constants ---

TEST_F(ParserTest, ParsesMultipleConstants) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '1';\n"
        "  B = '2,3';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    auto& declList1 = *constDecls.children()[0];
    EXPECT_EQ(declList1.data().rule, "7");

    // First constant: A
    auto& decl1 = *declList1.children()[0];
    auto& ident1 = *decl1.children()[0]->children()[0];
    EXPECT_EQ(ident1.data().symbol, "A");

    // Second list entry
    auto& declList2 = *declList1.children()[1];
    EXPECT_EQ(declList2.data().rule, "7");
    auto& decl2 = *declList2.children()[0];
    auto& ident2 = *decl2.children()[0]->children()[0];
    EXPECT_EQ(ident2.data().symbol, "B");

    // List terminator
    auto& declList3 = *declList2.children()[1];
    EXPECT_EQ(declList3.data().symbol, "<empty>");
}

// --- Comma-only right part (no left part) ---

TEST_F(ParserTest, ParsesCommaOnlyConstant) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = ',4';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().symbol, "<empty>");
    EXPECT_EQ(complexNum.children()[1]->data().symbol, ",<unsigned-integer>");
}

// --- $EXP-only (no left part) ---

TEST_F(ParserTest, ParsesExpOnlyConstant) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '$EXP(7)';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().symbol, "<empty>");
    EXPECT_EQ(
        complexNum.children()[1]->data().symbol, "$EXP( <unsigned-integer> )"
    );
}

// --- Missing symbol errors ---

TEST_F(ParserTest, LogsErrorForMissingProcedureIdentifier) {
    auto parser = makeParser("PROGRAM; BEGIN END.");
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(
        0, SyntaxError::ExpectedIdentifier
    );  // TODO: update after implementing constext specific identifier error
        // messages
}

TEST_F(ParserTest, LogsErrorForMissingEquals) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X '42';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedEquals);
}

TEST_F(ParserTest, LogsErrorForMissingClosingQuote) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  Y = '24;\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedClosingQuote);
}

TEST_F(ParserTest, LogsErrorForMissingConstantIdentifier) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  = '23,45';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedIdentifier);
}

TEST_F(ParserTest, LogsErrorForMissingSemicolonAfterConstant) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  Z = '54$EXP(32)'\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedConstantSemicolon);
}

TEST_F(ParserTest, LogsErrorForMissingBeginKeyword) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedBeginKeyword);
}

TEST_F(ParserTest, LogsErrorForMissingDot) {
    auto parser = makeParser("PROGRAM TEST; BEGIN END");
    parser.parse();

    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedDot);
}

TEST_F(ParserTest, RecoverFromMissingEqualsAndContinues) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X '42';\n"
        "  Y = '1';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    // First declaration fails, but second should still parse
    ASSERT_GE(logger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedEquals);

    // Tree should still contain the second constant
    auto& root = parser.tree().root();
    auto& programBody = *root.children()[0]->children()[0];
    auto& block = *programBody.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    // Should have parsed at least some declarations
    ASSERT_GE(constDecls.children().size(), 1);
}
