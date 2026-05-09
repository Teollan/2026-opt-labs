#include <gtest/gtest.h>

#include <AbstractSyntaxTree.hpp>
#include <CharacterAttributes.hpp>
#include <Declaration.hpp>
#include <DeclarationsTable.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <SemanticAnalyzer.hpp>
#include <SemanticError.hpp>
#include <StringSource.hpp>
#include <SymbolStore.hpp>
#include <SyntaxError.hpp>
#include <Tokenizer.hpp>
#include <complex>
#include <format>

struct AnalysisResult {
    std::vector<SemanticError> errors;
    DeclarationsTable declarations;
};

class SemanticAnalyzerTest : public ::testing::Test {
protected:
    SymbolStore symbols;
    CharacterAttributes attributes;
    std::ostringstream logOutput;

    Logger<LexicalError> tokenizerLogger{
        "Tokenizer",
        [](const LexicalError& err) {
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

    Logger<SemanticError> analyzerLogger{
        "Semantics",
        [](const SemanticError& err) {
            return std::format(
                "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
            );
        },
        logOutput,
    };

    void SetUp() override {
        tokenizerLogger.clear();
        parserLogger.clear();
        analyzerLogger.clear();
        logOutput.str("");
    }

    AnalysisResult analyze(const std::string& input) {
        StringSource source(input);

        Tokenizer tokenizer(source, symbols, attributes, tokenizerLogger);
        tokenizer.scan();

        Parser parser(symbols, tokenizer.tokens(), parserLogger);
        parser.parse();

        AbstractSyntaxTree ast(parser.tree(), symbols);

        SemanticAnalyzer analyzer(ast, analyzerLogger);
        analyzer.analyze();

        return {
            .errors = analyzerLogger.messages(),
            .declarations = analyzer.declarations(),
        };
    }
};

// --- Valid programs produce no errors ---

TEST_F(SemanticAnalyzerTest, NoErrorsForEmptyProgram) {
    auto [errors, _] = analyze("PROGRAM A; BEGIN END.");
    EXPECT_TRUE(errors.empty());
}

TEST_F(SemanticAnalyzerTest, NoErrorsForProgramWithOneConstant) {
    auto [errors, _] = analyze(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X = '42';\n"
        "BEGIN END."
    );
    EXPECT_TRUE(errors.empty());
}

TEST_F(SemanticAnalyzerTest, NoErrorsForMultipleUniqueConstants) {
    auto [errors, _] = analyze(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '1';\n"
        "  B = '2,3';\n"
        "  C = '4$EXP(5)';\n"
        "BEGIN END."
    );
    EXPECT_TRUE(errors.empty());
}

TEST_F(SemanticAnalyzerTest, NoErrorsForAllConstantFormats) {
    auto [errors, _] = analyze(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  A = '';\n"
        "  B = '1';\n"
        "  C = '2,3';\n"
        "  D = ',4';\n"
        "  E = '5$EXP(6)';\n"
        "  F = '$EXP(7)';\n"
        "BEGIN END."
    );
    EXPECT_TRUE(errors.empty());
}

// --- Duplicate constant declaration ---

TEST_F(SemanticAnalyzerTest, DuplicateConstantReportsOneError) {
    auto [errors, _] = analyze(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X = '1';\n"
        "  X = '2';\n"
        "BEGIN END."
    );
    EXPECT_EQ(errors.size(), 1u);
}

TEST_F(SemanticAnalyzerTest, DuplicateConstantErrorNamesTheIdentifier) {
    auto [errors, _] = analyze(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X = '1';\n"
        "  X = '2';\n"
        "BEGIN END."
    );
    ASSERT_EQ(errors.size(), 1u);
    EXPECT_EQ(errors[0].message, SemanticError::DuplicateIdentifier("X"));
}

TEST_F(SemanticAnalyzerTest, DuplicateConstantErrorPointsToSecondDeclaration) {
    auto [errors, _] = analyze(
        "PROGRAM TEST;\n"  // row 0
        "CONST\n"          // row 1
        "  X = '1';\n"     // row 2 — first declaration, accepted
        "  X = '2';\n"     // row 3, col 2  <-- duplicate, reported
        "BEGIN END."
    );
    ASSERT_EQ(errors.size(), 1u);
    EXPECT_EQ(errors[0].row, 3u);
    EXPECT_EQ(errors[0].column, 2u);
}

TEST_F(SemanticAnalyzerTest, DuplicateConstantOnlyAddsOneEntry) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  X = '1';\n"
        "  X = '2';\n"
        "BEGIN END."
    );
    EXPECT_NE(decls.lookup("X"), std::nullopt);
    EXPECT_EQ(decls.entries().size(), 2u);  // TEST + X
}

// --- Program name collision with constant ---

TEST_F(SemanticAnalyzerTest, ConstantNamedAfterProgramReportsOneError) {
    auto [errors, _] = analyze(
        "PROGRAM HELLO;\n"
        "CONST\n"
        "  HELLO = '42';\n"
        "BEGIN END."
    );
    EXPECT_EQ(errors.size(), 1u);
}

TEST_F(SemanticAnalyzerTest, ConstantNamedAfterProgramErrorNamesTheIdentifier) {
    auto [errors, _] = analyze(
        "PROGRAM HELLO;\n"
        "CONST\n"
        "  HELLO = '42';\n"
        "BEGIN END."
    );
    ASSERT_EQ(errors.size(), 1u);
    EXPECT_EQ(errors[0].message, SemanticError::DuplicateIdentifier("HELLO"));
}

TEST_F(SemanticAnalyzerTest, ConstantNamedAfterProgramErrorPointsToConstant) {
    auto [errors, _] = analyze(
        "PROGRAM HELLO;\n"   // row 0
        "CONST\n"            // row 1
        "  HELLO = '42';\n"  // row 2, col 2  <-- reported
        "BEGIN END."
    );
    ASSERT_EQ(errors.size(), 1u);
    EXPECT_EQ(errors[0].row, 2u);
    EXPECT_EQ(errors[0].column, 2u);
}

// --- Multiple redefinition errors ---

TEST_F(SemanticAnalyzerTest, MultipleRedefinitionsReportMultipleErrors) {
    auto [errors, _] = analyze(
        "PROGRAM HELLO;\n"
        "CONST\n"
        "  X = '1';\n"
        "  X = '2';\n"
        "  HELLO = '3,4';\n"
        "BEGIN END."
    );
    EXPECT_EQ(errors.size(), 2u);
}

TEST_F(SemanticAnalyzerTest, MultipleRedefinitionsNameCorrectIdentifiers) {
    auto [errors, _] = analyze(
        "PROGRAM HELLO;\n"
        "CONST\n"
        "  X = '1';\n"
        "  X = '2';\n"        // duplicate X reported first (source order)
        "  HELLO = '3,4';\n"  // duplicate HELLO reported second
        "BEGIN END."
    );
    ASSERT_EQ(errors.size(), 2u);
    EXPECT_EQ(errors[0].message, SemanticError::DuplicateIdentifier("X"));
    EXPECT_EQ(errors[1].message, SemanticError::DuplicateIdentifier("HELLO"));
}

// --- Declarations: kinds ---

TEST_F(SemanticAnalyzerTest, ProgramDeclarationHasKindProgram) {
    auto [_, decls] = analyze("PROGRAM MYPROG; BEGIN END.");
    auto decl = decls.lookup("MYPROG");
    ASSERT_TRUE(decl.has_value());
    EXPECT_EQ(decl->kind, DeclarationKind::Program);
}

TEST_F(SemanticAnalyzerTest, ConstantDeclarationHasKindConstant) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '1';\n"
        "BEGIN END."
    );
    auto decl = decls.lookup("X");
    ASSERT_TRUE(decl.has_value());
    EXPECT_EQ(decl->kind, DeclarationKind::Constant);
}

TEST_F(SemanticAnalyzerTest, ProgramDeclarationHasNoTypeOrValue) {
    auto [_, decls] = analyze("PROGRAM MYPROG; BEGIN END.");
    auto decl = decls.lookup("MYPROG");
    ASSERT_TRUE(decl.has_value());
    EXPECT_FALSE(decl->type.has_value());
    EXPECT_FALSE(decl->value.has_value());
}

// --- Declarations: types and values ---

TEST_F(SemanticAnalyzerTest, PlainIntegerConstantHasIntegerType) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '42';\n"
        "BEGIN END."
    );
    ASSERT_EQ(decls.lookup("X")->type, Type::Integer);
}

TEST_F(SemanticAnalyzerTest, CommaConstantHasIntegerType) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '2,3';\n"
        "BEGIN END."
    );
    ASSERT_EQ(decls.lookup("X")->type, Type::Integer);
}

TEST_F(SemanticAnalyzerTest, ExpConstantHasFloatType) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '5$EXP(6)';\n"
        "BEGIN END."
    );
    ASSERT_EQ(decls.lookup("X")->type, Type::Float);
}

TEST_F(SemanticAnalyzerTest, PlainIntegerConstantValueIsCorrect) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '42';\n"
        "BEGIN END."
    );
    auto val = decls.lookup("X")->value;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(std::get<std::complex<int>>(*val), (std::complex<int>{42, 0}));
}

TEST_F(SemanticAnalyzerTest, CommaConstantValueIsCorrect) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '2,3';\n"
        "BEGIN END."
    );
    auto val = decls.lookup("X")->value;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(std::get<std::complex<int>>(*val), (std::complex<int>{2, 3}));
}

TEST_F(SemanticAnalyzerTest, CommaOnlyConstantValueHasZeroRealPart) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = ',4';\n"
        "BEGIN END."
    );
    auto val = decls.lookup("X")->value;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(std::get<std::complex<int>>(*val), (std::complex<int>{0, 4}));
}

TEST_F(SemanticAnalyzerTest, EmptyConstantValueIsZero) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '';\n"
        "BEGIN END."
    );
    auto val = decls.lookup("X")->value;
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(std::get<std::complex<int>>(*val), (std::complex<int>{0, 0}));
}

TEST_F(SemanticAnalyzerTest, ExpConstantValueIsPolarForm) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '5$EXP(6)';\n"
        "BEGIN END."
    );
    auto val = decls.lookup("X")->value;
    ASSERT_TRUE(val.has_value());
    auto cval = std::get<std::complex<float>>(*val);
    auto expected = std::polar(5.0F, 6.0F);
    EXPECT_FLOAT_EQ(cval.real(), expected.real());
    EXPECT_FLOAT_EQ(cval.imag(), expected.imag());
}

// --- Declarations: modifiers and ordering ---

TEST_F(SemanticAnalyzerTest, ConstantHasComplexModifier) {
    auto [_, decls] = analyze(
        "PROGRAM TEST;\n"
        "CONST X = '1';\n"
        "BEGIN END."
    );
    auto decl = decls.lookup("X");
    ASSERT_TRUE(decl.has_value());
    ASSERT_EQ(decl->modifiers.size(), 1u);
    EXPECT_EQ(decl->modifiers[0], TypeModifier::Complex);
}

TEST_F(SemanticAnalyzerTest, DeclarationsAreSortedAlphabetically) {
    auto [_, decls] = analyze(
        "PROGRAM ZEBRA;\n"
        "CONST\n"
        "  BETA = '1';\n"
        "  ALPHA = '2';\n"
        "BEGIN END."
    );
    auto entries = decls.entries();
    ASSERT_EQ(entries.size(), 3u);
    EXPECT_EQ(entries[0].identifier, "ALPHA");
    EXPECT_EQ(entries[1].identifier, "BETA");
    EXPECT_EQ(entries[2].identifier, "ZEBRA");
}
