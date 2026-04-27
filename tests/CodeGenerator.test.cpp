#include <gtest/gtest.h>

#include <CharacterAttributes.hpp>
#include <CodeGenerator.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <SemanticAnalyzer.hpp>
#include <SemanticError.hpp>
#include <StringSource.hpp>
#include <SymbolStore.hpp>
#include <SyntaxError.hpp>
#include <Tokenizer.hpp>
#include <format>
#include <string>

class CodeGeneratorTest : public ::testing::Test {
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
        symbols = SymbolStore{};
        tokenizerLogger.clear();
        parserLogger.clear();
        analyzerLogger.clear();
        logOutput.str("");
    }

    std::string compile(const std::string& input) {
        StringSource source(input);
        Tokenizer tokenizer(source, symbols, attributes, tokenizerLogger);
        tokenizer.scan();
        Parser parser(symbols, tokenizer.tokens(), parserLogger);
        parser.parse();
        SemanticAnalyzer analyzer(symbols, parser.tree(), analyzerLogger);
        analyzer.analyze();
        CodeGenerator codegen(analyzer.declarations());
        codegen.generate();
        return codegen.output();
    }

    static bool has(const std::string& output, const std::string& fragment) {
        return output.contains(fragment);
    }

    static size_t pos(const std::string& output, const std::string& fragment) {
        return output.find(fragment);
    }
};

// --- Text section structure ---

TEST_F(CodeGeneratorTest, TextSectionIsPresent) {
    EXPECT_TRUE(has(compile("PROGRAM A; BEGIN END."), "section .text"));
}

TEST_F(CodeGeneratorTest, EntryPointDeclaredGlobal) {
    EXPECT_TRUE(has(compile("PROGRAM A; BEGIN END."), "global  _start"));
}

TEST_F(CodeGeneratorTest, EntryPointHasStandaloneLabel) {
    EXPECT_TRUE(has(compile("PROGRAM A; BEGIN END."), "_start:"));
}

TEST_F(CodeGeneratorTest, GlobalPrecedesStandaloneLabel) {
    const auto asm_ = compile("PROGRAM A; BEGIN END.");
    EXPECT_LT(pos(asm_, "global"), pos(asm_, "_start:"));
}

TEST_F(CodeGeneratorTest, NopEmittedForEmptyStatementsList) {
    EXPECT_TRUE(has(compile("PROGRAM A; BEGIN END."), "nop"));
}

TEST_F(CodeGeneratorTest, StandaloneLabelPrecedesNop) {
    const auto asm_ = compile("PROGRAM A; BEGIN END.");
    EXPECT_LT(pos(asm_, "_start:"), pos(asm_, "nop"));
}

TEST_F(CodeGeneratorTest, ExitsViaSyscall) {
    const auto asm_ = compile("PROGRAM A; BEGIN END.");
    EXPECT_TRUE(has(asm_, "syscall"));
    EXPECT_LT(pos(asm_, "nop"), pos(asm_, "syscall"));
}

// --- Data section: absent when no constants ---

TEST_F(CodeGeneratorTest, NoDataSectionForEmptyProgram) {
    EXPECT_FALSE(has(compile("PROGRAM A; BEGIN END."), "section .data"));
}

// --- Data section: present with constants ---

TEST_F(CodeGeneratorTest, DataSectionPresentWithConstants) {
    const auto asm_ = compile("PROGRAM T; CONST X = '1'; BEGIN END.");
    EXPECT_TRUE(has(asm_, "section .data"));
}

TEST_F(CodeGeneratorTest, DataSectionPrecedesTextSection) {
    const auto asm_ = compile("PROGRAM T; CONST X = '1'; BEGIN END.");
    EXPECT_LT(pos(asm_, "section .data"), pos(asm_, "section .text"));
}

// --- Constant directives ---

TEST_F(CodeGeneratorTest, ConstantUsesDoubleDwordDirective) {
    const auto asm_ = compile("PROGRAM T; CONST X = '1'; BEGIN END.");
    EXPECT_TRUE(has(asm_, "dd"));
}

TEST_F(CodeGeneratorTest, ConstantLabelAppearsInOutput) {
    const auto asm_ = compile("PROGRAM T; CONST MYCONST = '1'; BEGIN END.");
    EXPECT_TRUE(has(asm_, "MYCONST"));
}

TEST_F(CodeGeneratorTest, MultipleConstantsAllPresent) {
    const auto asm_ = compile(
        "PROGRAM T;\n"
        "CONST\n"
        "  ALPHA = '1,2';\n"
        "  BETA  = '3,4';\n"
        "BEGIN END."
    );
    EXPECT_TRUE(has(asm_, "ALPHA"));
    EXPECT_TRUE(has(asm_, "BETA"));
}

TEST_F(CodeGeneratorTest, ConstantsAppearBeforeTextSection) {
    const auto asm_ = compile(
        "PROGRAM T;\n"
        "CONST X = '1';\n"
        "BEGIN END."
    );
    EXPECT_LT(pos(asm_, "X"), pos(asm_, "section .text"));
}

// --- Constant values: integer complex ---

TEST_F(CodeGeneratorTest, RealOnlyConstantHasZeroImaginary) {
    EXPECT_TRUE(has(compile("PROGRAM T; CONST X = '3'; BEGIN END."), "3, 0"));
}

TEST_F(CodeGeneratorTest, ImaginaryOnlyConstantHasZeroReal) {
    EXPECT_TRUE(has(compile("PROGRAM T; CONST X = ',4'; BEGIN END."), "0, 4"));
}

TEST_F(CodeGeneratorTest, BothPartsEmittedCorrectly) {
    EXPECT_TRUE(has(compile("PROGRAM T; CONST X = '2,5'; BEGIN END."), "2, 5"));
}

TEST_F(CodeGeneratorTest, EmptyConstantEmittedAsZeroPair) {
    EXPECT_TRUE(has(compile("PROGRAM T; CONST X = ''; BEGIN END."), "0, 0"));
}

// --- Constant values: float complex (polar form) ---

TEST_F(CodeGeneratorTest, PolarConstantUsesDoubleDwordDirective) {
    const auto asm_ = compile("PROGRAM T; CONST X = '1$EXP(0)'; BEGIN END.");
    EXPECT_TRUE(has(asm_, "dd"));
}

TEST_F(CodeGeneratorTest, PolarConstantWithZeroExponentEmitsUnitReal) {
    // polar(1, 0) = {cos(0), sin(0)} = {1, 0}
    const auto asm_ = compile("PROGRAM T; CONST X = '1$EXP(0)'; BEGIN END.");
    EXPECT_TRUE(has(asm_, "1"));
    EXPECT_TRUE(has(asm_, "0"));
}
