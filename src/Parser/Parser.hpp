#pragma once

#include <Stack.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>
#include <Tree.hpp>

namespace SyntaxError {
    constexpr auto MustStartWithProgram = "Program must start with 'PROGRAM' keyword";
    constexpr auto ExpectedProcedureIdentifier = "Expected procedure identifier after 'PROGRAM' keyword";
    constexpr auto ExpectedSemicolon = "Expected ';' after procedure identifier";
    constexpr auto ExpectedDot = "Expected '.' at the end of the program";
    constexpr auto ExpectedBeginKeyword = "Expected 'BEGIN' keyword before statements list";
    constexpr auto ExpectedEndKeyword = "Expected 'END' keyword at the end of the block";
    constexpr auto ExpectedConstKeyword = "Expected 'CONST' keyword before constant declarations";
    constexpr auto ExpectedIdentifier = "Expected identifier in constant declaration";
    constexpr auto ExpectedEquals = "Expected '=' in constant declaration";
    constexpr auto ExpectedConstantSemicolon = "Expected ';' at the end of constant declaration";
    constexpr auto ExpectedOpeningQuote = "Expected ''' (single quote) at the beginning of constant declaration";
    constexpr auto ExpectedClosingQuote = "Expected ''' (single quote) at the end of constant declaration";
    constexpr auto ExpectedUnsignedInteger = "Expected unsigned integer literal";
    constexpr auto ExpectedComma = "Expected ',' in complex number";
    constexpr auto ExpectedExp = "Expected '$EXP' in complex number";
    constexpr auto ExpectedOpenParen = "Expected '(' after '$EXP' in complex number";
    constexpr auto ExpectedCloseParen = "Expected ')' after exponent in complex number";
}

class Parser {
private:
    const SymbolStore& symbols;

    Stack<Token> tokens;

    Tree<std::string> _tree;
    Stack<TreeNode<std::string>*> _nodeStack;

    void parseProgram();
    void parseBlock();
    void parseStatementsList();
    void parseConstantDeclarations();
    void parseConstantDeclarationsList();
    void parseConstantDeclaration();
    void parseConstant();
    void parseComplexNumber();
    void parseLeftPart();
    void parseRightPart();
    void parseUnsignedInteger();

    void expect(const std::string& expected, const std::string& errorMessage);
    void expect(SymbolType expected, const std::string& errorMessage);

    bool consider(const std::string& expected);
    bool consider(SymbolType expected);

    void panic(const std::string& message);
    void panic(const std::string& message, const Token& token);

    void grow(const std::string& data);

public:
    Parser(const SymbolStore& symbols, const std::vector<Token>& tokens);

    void parse();
    const Tree<std::string>& tree() const;
};