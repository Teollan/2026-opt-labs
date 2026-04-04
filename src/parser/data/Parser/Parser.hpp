#pragma once

#include <Logger.hpp>
#include <Stack.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>
#include <Tree.hpp>
#include <parser.constants.hpp>

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