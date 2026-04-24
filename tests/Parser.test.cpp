#include <gtest/gtest.h>

#include <CharacterAttributes.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <Rules.hpp>
#include <StringSource.hpp>
#include <SymbolStore.hpp>
#include <SyntaxError.hpp>
#include <Tokenizer.hpp>
#include <format>

class ParserTest : public ::testing::Test {
protected:
    SymbolStore symbols;
    CharacterAttributes attributes;
    std::ostringstream logOutput;

    Logger<Error> tokenizerLogger{
        "Tokenizer",
        [](const Error& err) {
            return std::format(
                "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
            );
        },
        logOutput,
    };

    Logger<SyntaxError> parserLogger{
        "Parser",
        [](const SyntaxError& err) {
            return std::format(
                "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
            );
        },
        logOutput,
    };

    void SetUp() override {
        tokenizerLogger.clear();
        parserLogger.clear();
        logOutput.str("");
    }

    void expectError(size_t index, const std::string& message) {
        ASSERT_LT(index, parserLogger.messages().size())
            << "Error index " << index << " out of range";
        EXPECT_EQ(parserLogger.messages()[index].message, message)
            << "Error " << index << " message mismatch";
    }

    Parser makeParser(const std::string& input) {
        StringSource source(input);
        Tokenizer tokenizer(source, symbols, attributes, tokenizerLogger);
        tokenizer.scan();
        return Parser(symbols, tokenizer.tokens(), parserLogger);
    }

    std::string tokenText(const TreeNode<SyntaxData>& node) {
        return symbols.lookup(node.data().token->code);
    }
};

// --- Empty program (no constants) ---

TEST_F(ParserTest, ParsesEmptyProgram) {
    auto parser = makeParser("PROGRAM EMPTY; BEGIN END.");
    parser.parse();

    const auto& root = parser.tree().root();
    EXPECT_EQ(root.data().rule, RuleKey::Axiom);

    // <signal-program> --> <program>
    auto& signalProgram = *root.children()[0];
    EXPECT_EQ(signalProgram.data().rule, RuleKey::SignalProgram);
    ASSERT_EQ(signalProgram.children().size(), 1);

    // <program> --> PROGRAM <procedure-identifier>; <block>.
    // 2 nonterminals: <procedure-identifier>, <block>
    auto& program = *signalProgram.children()[0];
    EXPECT_EQ(program.data().rule, RuleKey::Program);
    ASSERT_EQ(program.children().size(), 2);

    // <block> --> <declarations> BEGIN <statements-list> END
    // 2 nonterminals: <declarations>, <statements-list>
    auto& block = *program.children()[1];
    EXPECT_EQ(block.data().rule, RuleKey::Block);
    ASSERT_EQ(block.children().size(), 2);

    // <declarations> --> <constant-declarations> — 1 nonterminal
    auto& declarations = *block.children()[0];
    EXPECT_EQ(declarations.data().rule, RuleKey::Declarations);
    ASSERT_EQ(declarations.children().size(), 1);

    // <constant-declarations> --> <empty>
    auto& constDecls = *declarations.children()[0];
    EXPECT_EQ(constDecls.data().rule, RuleKey::ConstantDeclarationsEmpty);

    // <statements-list> --> <empty>
    auto& stmtList = *block.children()[1];
    EXPECT_EQ(stmtList.data().rule, RuleKey::StatementsList);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& declarations = *block.children()[0];

    // CONST <constant-declarations-list> — 1 nonterminal
    auto& constDecls = *declarations.children()[0];
    EXPECT_EQ(constDecls.data().rule, RuleKey::ConstantDeclarations);
    ASSERT_EQ(constDecls.children().size(), 1);

    // <constant-declaration> <constant-declarations-list> — 2 nonterminals
    auto& declList = *constDecls.children()[0];
    EXPECT_EQ(declList.data().rule, RuleKey::ConstantDeclarationsList);
    ASSERT_EQ(declList.children().size(), 2);

    // <constant-identifier> = <constant>; — 2 nonterminals
    auto& decl = *declList.children()[0];
    EXPECT_EQ(decl.data().rule, RuleKey::ConstantDeclaration);
    ASSERT_EQ(decl.children().size(), 2);

    // ConstantIdentifier -> Terminal with identifier "X"
    auto& constId = *decl.children()[0];
    EXPECT_EQ(constId.data().rule, RuleKey::ConstantIdentifier);
    auto& ident = *constId.children()[0];
    EXPECT_EQ(ident.data().rule, RuleKey::Terminal);
    EXPECT_EQ(tokenText(ident), "X");

    // '<complex-number>' — 1 nonterminal
    auto& constant = *decl.children()[1];
    EXPECT_EQ(constant.data().rule, RuleKey::Constant);
    ASSERT_EQ(constant.children().size(), 1);

    // <left-part> <right-part>
    auto& complexNum = *constant.children()[0];
    EXPECT_EQ(complexNum.data().rule, RuleKey::ComplexNumber);
    ASSERT_EQ(complexNum.children().size(), 2);

    // <left-part> --> <unsigned-integer>
    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().rule, RuleKey::LeftPartValue);

    // <right-part> --> <empty>
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().rule, RuleKey::RightPartEmpty);

    // list ends with <empty>
    auto& declListEnd = *declList.children()[1];
    EXPECT_EQ(declListEnd.data().rule, RuleKey::ConstantDeclarationsListEmpty);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().rule, RuleKey::LeftPartValue);

    // ,<unsigned-integer> — 1 nonterminal (Terminal child)
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().rule, RuleKey::RightPartBase);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().rule, RuleKey::LeftPartValue);

    // $EXP( <unsigned-integer> ) — 1 nonterminal
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().rule, RuleKey::RightPartExp);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().rule, RuleKey::LeftPartEmpty);
    EXPECT_EQ(complexNum.children()[1]->data().rule, RuleKey::RightPartEmpty);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    auto& declList1 = *constDecls.children()[0];
    EXPECT_EQ(declList1.data().rule, RuleKey::ConstantDeclarationsList);

    // First constant: A
    auto& decl1 = *declList1.children()[0];
    auto& ident1 = *decl1.children()[0]->children()[0];
    EXPECT_EQ(tokenText(ident1), "A");

    // Second list entry
    auto& declList2 = *declList1.children()[1];
    EXPECT_EQ(declList2.data().rule, RuleKey::ConstantDeclarationsList);
    auto& decl2 = *declList2.children()[0];
    auto& ident2 = *decl2.children()[0]->children()[0];
    EXPECT_EQ(tokenText(ident2), "B");

    // List terminator
    auto& declList3 = *declList2.children()[1];
    EXPECT_EQ(declList3.data().rule, RuleKey::ConstantDeclarationsListEmpty);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().rule, RuleKey::LeftPartEmpty);
    EXPECT_EQ(complexNum.children()[1]->data().rule, RuleKey::RightPartBase);
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

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().rule, RuleKey::LeftPartEmpty);
    EXPECT_EQ(complexNum.children()[1]->data().rule, RuleKey::RightPartExp);
}

// --- Missing symbol errors ---

TEST_F(ParserTest, LogsErrorForMissingProcedureIdentifier) {
    auto parser = makeParser("PROGRAM; BEGIN END.");
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedProcedureIdentifier);
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

    ASSERT_GE(parserLogger.messages().size(), 1);
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

    ASSERT_GE(parserLogger.messages().size(), 1);
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

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedConstantIdentifier);
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

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedConstantSemicolon);
}

TEST_F(ParserTest, LogsErrorForMissingBeginKeyword) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedBeginKeyword);
}

TEST_F(ParserTest, LogsErrorForMissingDot) {
    auto parser = makeParser("PROGRAM TEST; BEGIN END");
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
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
    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedEquals);

    // Tree should still contain the second constant
    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    // Should have parsed at least some declarations
    ASSERT_GE(constDecls.children().size(), 1);
}

// --- Edge cases: empty and minimal inputs ---

TEST_F(ParserTest, HandlesCompletelyEmptyInput) {
    auto parser = makeParser("");
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::MustStartWithProgram);
}

TEST_F(ParserTest, HandlesProgramKeywordOnly) {
    auto parser = makeParser("PROGRAM");
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedProcedureIdentifier);
}

TEST_F(ParserTest, HandlesMinimalValidProgram) {
    auto parser = makeParser("PROGRAM A; BEGIN END.");
    parser.parse();

    EXPECT_EQ(parserLogger.messages().size(), 0);
}

// --- Edge cases: constant values ---

TEST_F(ParserTest, ParsesConstantWithBothParts) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '10,20';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    EXPECT_EQ(parserLogger.messages().size(), 0);

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    // Left part has unsigned integer (Terminal child)
    auto& leftPart = *complexNum.children()[0];
    EXPECT_EQ(leftPart.data().rule, RuleKey::LeftPartValue);
    ASSERT_EQ(leftPart.children().size(), 1);
    EXPECT_EQ(tokenText(*leftPart.children()[0]), "10");

    // Right part is comma variant with unsigned integer (Terminal child)
    auto& rightPart = *complexNum.children()[1];
    EXPECT_EQ(rightPart.data().rule, RuleKey::RightPartBase);
    ASSERT_EQ(rightPart.children().size(), 1);
    EXPECT_EQ(tokenText(*rightPart.children()[0]), "20");
}

TEST_F(ParserTest, ParsesConstantWithExpAndLeftPart) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '3$EXP(5)';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    EXPECT_EQ(parserLogger.messages().size(), 0);

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& decl =
        *block.children()[0]->children()[0]->children()[0]->children()[0];
    auto& complexNum = *decl.children()[1]->children()[0];

    EXPECT_EQ(complexNum.children()[0]->data().rule, RuleKey::LeftPartValue);
    EXPECT_EQ(complexNum.children()[1]->data().rule, RuleKey::RightPartExp);
}

// --- Edge cases: recovery behavior ---

TEST_F(ParserTest, RecoverFromMissingSemicolonImplicitly) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '1'\n"
        "  B = '2';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    // Missing semicolon after A should be reported
    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedConstantSemicolon);

    // B should still parse successfully after implicit semicolon insertion
    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    auto& declList1 = *constDecls.children()[0];

    // First declaration (A) should exist
    ASSERT_GE(declList1.children().size(), 2);

    // Second list entry should contain B
    auto& declList2 = *declList1.children()[1];
    ASSERT_GE(declList2.children().size(), 1);
    auto& decl2 = *declList2.children()[0];
    auto& ident2 = *decl2.children()[0]->children()[0];
    EXPECT_EQ(tokenText(ident2), "B");
}

TEST_F(ParserTest, RecoverFromMissingClosingQuoteAndContinues) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '42;\n"
        "  B = '99';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedClosingQuote);

    // B should still parse after recovery
    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    ASSERT_GE(constDecls.children().size(), 1);
}

TEST_F(ParserTest, RecoverFromBadProcedureHeaderAndParseBlock) {
    auto parser = makeParser(
        "PROGRAM;\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    // Procedure identifier missing
    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::ExpectedProcedureIdentifier);

    // Block should still parse after header recovery
    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    // Should have at least the block child
    ASSERT_GE(program.children().size(), 1);
}

TEST_F(ParserTest, MultipleDeclarationErrorsRecoverIndependently) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X '1';\n"
        "  Y '2';\n"
        "  Z = '3';\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    // Both X and Y should report missing '='
    ASSERT_GE(parserLogger.messages().size(), 2);
    expectError(0, SyntaxError::ExpectedEquals);
    expectError(1, SyntaxError::ExpectedEquals);

    // Z should still parse correctly
    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& constDecls = *block.children()[0]->children()[0];
    ASSERT_GE(constDecls.children().size(), 1);
}

// --- Edge cases: unexpected end of file ---

TEST_F(ParserTest, HandlesMissingEndAndDot) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "BEGIN"
    );
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
}

TEST_F(ParserTest, HandlesMissingBlockEntirely) {
    auto parser = makeParser("PROGRAM TEST;");
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
}

// --- Edge cases: extraneous symbols ---

TEST_F(ParserTest, LogsErrorForExtraTokensAfterDot) {
    auto parser = makeParser("PROGRAM TEST; BEGIN END. EXTRA");
    parser.parse();

    ASSERT_GE(parserLogger.messages().size(), 1);
    expectError(0, SyntaxError::UnexpectedSymbolsAfterEndOfProgram);
}

TEST_F(ParserTest, ParsesNoConstantsWithBeginDirectly) {
    auto parser = makeParser(
        "PROGRAM TEST;\n"
        "BEGIN\n"
        "END."
    );
    parser.parse();

    EXPECT_EQ(parserLogger.messages().size(), 0);

    const auto& root = parser.tree().root();
    auto& program = *root.children()[0]->children()[0];
    auto& block = *program.children()[1];
    auto& declarations = *block.children()[0];
    auto& constDecls = *declarations.children()[0];
    EXPECT_EQ(constDecls.data().rule, RuleKey::ConstantDeclarationsEmpty);
}
