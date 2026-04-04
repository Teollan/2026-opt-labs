#pragma once

#include <Logger.hpp>
#include <Stack.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>
#include <Tree.hpp>

namespace SyntaxError {
constexpr auto MustStartWithProgram =
    "Program must start with 'PROGRAM' keyword";
constexpr auto ExpectedProcedureIdentifier =
    "Expected procedure identifier after 'PROGRAM' keyword";
constexpr auto ExpectedSemicolon = "Expected ';' after procedure identifier";
constexpr auto ExpectedDot = "Expected '.' at the end of the program";
constexpr auto ExpectedBeginKeyword =
    "Expected 'BEGIN' keyword before statements list";
constexpr auto ExpectedEndKeyword =
    "Expected 'END' keyword at the end of the block";
constexpr auto ExpectedConstKeyword =
    "Expected 'CONST' keyword before constant declarations";
constexpr auto ExpectedIdentifier = "Expected identifier";
constexpr auto ExpectedEquals = "Expected '=' in constant declaration";
constexpr auto ExpectedConstantSemicolon =
    "Expected ';' at the end of constant declaration";
constexpr auto ExpectedOpeningQuote =
    "Expected ''' (single quote) at the beginning of constant declaration";
constexpr auto ExpectedClosingQuote =
    "Expected ''' (single quote) at the end of constant declaration";
constexpr auto ExpectedUnsignedInteger = "Expected unsigned integer literal";
constexpr auto ExpectedComma = "Expected ',' in complex number";
constexpr auto ExpectedExp = "Expected '$EXP' in complex number";
constexpr auto ExpectedOpenParen =
    "Expected '(' after '$EXP' in complex number";
constexpr auto ExpectedCloseParen =
    "Expected ')' after exponent in complex number";
constexpr auto UnexpectedEndOfFile = "Unexpected end of file";
}  // namespace SyntaxError

struct SyntaxData {
    std::string symbol;
    std::string rule;
};

class Parser {
private:
    const SymbolStore& symbols;
    Logger<Error>& _logger;

    Stack<Token> tokens;

    Tree<SyntaxData> _tree;
    Stack<TreeNode<SyntaxData>*> _nodeStack;

    // 1. <signal-program> --> <program>
    void parseSignalProgram();
    // 2. <program> --> PROGRAM <procedure-identifier>; <block>.
    void parseProgram();
    // 3. <block> --> <declarations> BEGIN <statements-list> END
    void parseBlock();
    // 4. <statements-list> --> <empty>
    void parseStatementsList();
    // 5. <declarations> --> <constant-declarations>
    void parseDeclarations();
    // 6. <constant-declarations> --> CONST <constant-declarations-list> |
    // <empty>
    void parseConstantDeclarations();
    // 7. <constant-declarations-list> --> <constant-declaration>
    // <constant-declarations-list> | <empty>
    void parseConstantDeclarationsList();
    // 8. <constant-declaration> --> <constant-identifier> = <constant>;
    void parseConstantDeclaration();
    // 9. <constant> --> '<complex-number>'
    void parseConstant();
    // 10. <complex-number> --> <left-part> <right-part>
    void parseComplexNumber();
    // 11. <left-part> --> <unsigned-integer> | <empty>
    void parseLeftPart();
    // 12. <right-part> --> ,<unsigned-integer> | $EXP( <unsigned-integer> ) |
    // <empty>
    void parseRightPart();
    // 13. <constant-identifier> --> <identifier>
    void parseConstantIdentifier();
    // 14. <procedure-identifier> --> <identifier>
    void parseProcedureIdentifier();
    // 15. <identifier> --> <letter><string>
    void parseIdentifier();
    // 17. <unsigned-integer> --> <digit><digits-string>
    void parseUnsignedInteger();

    void recoverFromParseProgramPanic(TreeNode<SyntaxData>& parentNode);
    void recoverFromParseProcedureHeaderPanic(TreeNode<SyntaxData>& parentNode);
    void recoverFromParseBlockPanic(TreeNode<SyntaxData>& parentNode);
    void recoverFromParseConstantDeclarationPanic(TreeNode<SyntaxData>& node);

    Token expect(const std::string& expected, const std::string& errorMessage);
    Token expect(SymbolType expected, const std::string& errorMessage);

    bool consider(const std::string& expected);
    bool consider(SymbolType expected);

    void panic(const std::string& message);
    void panic(const std::string& message, const Token& token);

    TreeNode<SyntaxData>& grow(const SyntaxData& data);

public:
    Parser(
        const SymbolStore& symbols,
        const std::vector<Token>& tokens,
        Logger<Error>& logger
    );

    void parse();
    const Tree<SyntaxData>& tree() const;
};