#include "Parser.hpp"

#include <format>
#include <stdexcept>

Parser::Parser(
    const SymbolStore& symbols,
    const std::vector<Token>& tokens,
    Logger<Error>& logger
) :
    symbols(symbols),
    _logger(logger),
    tokens(Stack<Token>({tokens.rbegin(), tokens.rend()})),
    _tree(Tree<SyntaxData>({.symbol = "<signal-program>", .rule = "S"})) {
    _nodeStack.push(&_tree.root());
}

void Parser::parse() {
    parseSignalProgram();
}

// 1. <signal-program> --> <program>
void Parser::parseSignalProgram() {
    auto& node = grow({
        .symbol = "<program>",
        .rule = "1",
    });

    try {
        parseProgram();
    } catch (const std::runtime_error& error) {
        recoverFromParseProgramPanic(node);
    }

    _nodeStack.pop();
}

// 2. <program> --> PROGRAM <procedure-identifier>; <block>.
void Parser::parseProgram() {
    auto& node = grow({
        .symbol = "PROGRAM <procedure-identifier>; <block>.",
        .rule = "2",
    });

    try {
        expect(Keyword::Program, SyntaxError::MustStartWithProgram);
        parseProcedureIdentifier();
        expect(Delimiter::Semicolon, SyntaxError::ExpectedSemicolon);
    } catch (const std::runtime_error& error) {
        recoverFromParseProcedureHeaderPanic(node);
    }

    try {
        parseBlock();
    } catch (const std::runtime_error& error) {
        recoverFromParseBlockPanic(node);
    }

    expect(Delimiter::Dot, SyntaxError::ExpectedDot);

    _nodeStack.pop();
}

// 3. <block> --> <declarations> BEGIN <statements-list> END
void Parser::parseBlock() {
    grow({
        .symbol = "<declarations> BEGIN <statements-list> END",
        .rule = "3",
    });

    parseDeclarations();
    expect(Keyword::Begin, SyntaxError::ExpectedBeginKeyword);
    parseStatementsList();
    expect(Keyword::End, SyntaxError::ExpectedEndKeyword);

    _nodeStack.pop();
}

// 4. <statements-list> --> <empty>
void Parser::parseStatementsList() {
    grow({
        .symbol = "<empty>",
        .rule = "4",
    });

    _nodeStack.pop();
}

// 5. <declarations> --> <constant-declarations>
void Parser::parseDeclarations() {
    grow({
        .symbol = "<constant-declarations>",
        .rule = "5",
    });

    parseConstantDeclarations();

    _nodeStack.pop();
}

// 6. <constant-declarations> --> CONST <constant-declarations-list> | <empty>
void Parser::parseConstantDeclarations() {
    auto isProvided = consider(Keyword::Const);

    if (isProvided) {
        grow({
            .symbol = "CONST <constant-declarations-list>",
            .rule = "6",
        });

        expect(Keyword::Const, SyntaxError::ExpectedConstKeyword);
        parseConstantDeclarationsList();
    } else {
        grow({
            .symbol = "<empty>",
            .rule = "6",
        });
    }

    _nodeStack.pop();
}

// 7. <constant-declarations-list> --> <constant-declaration>
// <constant-declarations-list> | <empty>
void Parser::parseConstantDeclarationsList() {
    auto isEmpty = consider(Keyword::Begin);

    if (!isEmpty) {
        auto& node = grow({
            .symbol = "<constant-declaration> <constant-declarations-list>",
            .rule = "7",
        });

        try {
            parseConstantDeclaration();
        } catch (const std::runtime_error& error) {
            recoverFromParseConstantDeclarationPanic(node);
        }

        parseConstantDeclarationsList();
    } else {
        grow({
            .symbol = "<empty>",
            .rule = "7",
        });
    }

    _nodeStack.pop();
}

// 8. <constant-declaration> --> <constant-identifier> = <constant>;
void Parser::parseConstantDeclaration() {
    grow({
        .symbol = "<constant-identifier> = <constant>;",
        .rule = "8",
    });

    parseConstantIdentifier();
    expect(Delimiter::Equals, SyntaxError::ExpectedEquals);
    parseConstant();
    expect(Delimiter::Semicolon, SyntaxError::ExpectedConstantSemicolon);

    _nodeStack.pop();
}

// 9. <constant> --> '<complex-number>'
void Parser::parseConstant() {
    grow({
        .symbol = "'<complex-number>'",
        .rule = "9",
    });

    expect(Delimiter::Quote, SyntaxError::ExpectedOpeningQuote);
    parseComplexNumber();
    expect(Delimiter::Quote, SyntaxError::ExpectedClosingQuote);

    _nodeStack.pop();
}

// 10. <complex-number> --> <left-part> <right-part>
void Parser::parseComplexNumber() {
    grow({
        .symbol = "<left-part> <right-part>",
        .rule = "10",
    });

    parseLeftPart();
    parseRightPart();

    _nodeStack.pop();
}

// 11. <left-part> --> <unsigned-integer> | <empty>
void Parser::parseLeftPart() {
    auto isProvided = consider(SymbolType::Literal);

    if (isProvided) {
        grow({
            .symbol = "<unsigned-integer>",
            .rule = "11",
        });

        parseUnsignedInteger();
    } else {
        grow({
            .symbol = "<empty>",
            .rule = "11",
        });
    }

    _nodeStack.pop();
}

// 12. <right-part> --> ,<unsigned-integer> | $EXP( <unsigned-integer> ) |
// <empty>
void Parser::parseRightPart() {
    auto isCommaVariant = consider(Delimiter::Comma);

    if (isCommaVariant) {
        grow({
            .symbol = ",<unsigned-integer>",
            .rule = "12",
        });

        expect(Delimiter::Comma, SyntaxError::ExpectedComma);
        parseUnsignedInteger();

        _nodeStack.pop();
        return;
    }

    auto isExpVariant = consider(MultiDelimiter::Exp);

    if (isExpVariant) {
        grow({
            .symbol = "$EXP( <unsigned-integer> )",
            .rule = "12",
        });

        expect(MultiDelimiter::Exp, SyntaxError::ExpectedExp);
        expect(Delimiter::OpenParenthesis, SyntaxError::ExpectedOpenParen);
        parseUnsignedInteger();
        expect(Delimiter::CloseParenthesis, SyntaxError::ExpectedCloseParen);

        _nodeStack.pop();
        return;
    }

    grow({
        .symbol = "<empty>",
        .rule = "12",
    });

    _nodeStack.pop();
}

// 13. <constant-identifier> --> <identifier>
void Parser::parseConstantIdentifier() {
    grow({
        .symbol = "<identifier>",
        .rule = "13",
    });

    parseIdentifier();

    _nodeStack.pop();
}

// 14. <procedure-identifier> --> <identifier>
void Parser::parseProcedureIdentifier() {
    grow({
        .symbol = "<identifier>",
        .rule = "14",
    });

    parseIdentifier();

    _nodeStack.pop();
}

// 15. <identifier> --> <letter><string>
void Parser::parseIdentifier() {
    auto token =
        expect(SymbolType::Identifier, SyntaxError::ExpectedIdentifier);

    grow({
        .symbol = symbols.lookup(token.code),
        .rule = "15",
    });

    _nodeStack.pop();
}

// 17. <unsigned-integer> --> <digit><digits-string>
void Parser::parseUnsignedInteger() {
    auto token =
        expect(SymbolType::Literal, SyntaxError::ExpectedUnsignedInteger);

    grow({
        .symbol = symbols.lookup(token.code),
        .rule = "17",
    });

    _nodeStack.pop();
}

void Parser::recoverFromParseProgramPanic(TreeNode<SyntaxData>& parentNode) {
    // If this is reached, the program is fucked
    // Clear all tokens and pop all nodes until the root
    tokens.clear();

    _nodeStack.popUntil([&](TreeNode<SyntaxData>* node) {
        return node == &parentNode;
    });
}

void Parser::recoverFromParseProcedureHeaderPanic(
    TreeNode<SyntaxData>& parentNode
) {
    // Pop tokens until we find a token to continue parsing from.
    auto referenceToken = tokens.popUntil([&](const Token& token) {
        const auto& symbol = symbols.lookup(token.code);

        return symbol == Keyword::Const || symbol == Keyword::Begin;
    });

    // Popped all tokens but didn't find a reference,
    // which means we reached the end of the file.
    if (!referenceToken) {
        panic(SyntaxError::UnexpectedEndOfFile);
    }

    // Clear the node stack until the parent node
    _nodeStack.popUntil([&](TreeNode<SyntaxData>* node) {
        return node == &parentNode;
    });
}

void Parser::recoverFromParseBlockPanic(TreeNode<SyntaxData>& parentNode) {
    // Pop tokens until we find a token to continue parsing from.
    auto referenceToken = tokens.popUntil([&](const Token& token) {
        const auto& symbol = symbols.lookup(token.code);

        return symbol == Keyword::End || symbol == Delimiter::Dot;
    });

    // Popped all tokens but didn't find a reference,
    // which means we reached the end of the file.
    if (!referenceToken) {
        panic(SyntaxError::UnexpectedEndOfFile);
    }

    // If the safe token is an END keyword, we need to consume it
    if (symbols.lookup(referenceToken->code) == Keyword::End) {
        tokens.pop();
    }

    // Clear the node stack until the parent node
    _nodeStack.popUntil([&](TreeNode<SyntaxData>* node) {
        return node == &parentNode;
    });
}

void Parser::recoverFromParseConstantDeclarationPanic(
    TreeNode<SyntaxData>& parentNode
) {
    // Pop tokens until we find a token to continue parsing from.
    auto referenceToken = tokens.popUntil([&](const Token& token) {
        const auto& symbol = symbols.lookup(token.code);

        return symbol == Delimiter::Semicolon || symbol == Keyword::Begin;
    });

    // Popped all tokens but didn't find a reference,
    // which means we reached the end of the file.
    if (!referenceToken) {
        panic(SyntaxError::UnexpectedEndOfFile);
    }

    // If the safe token is a semicolon, we need to consume it
    if (symbols.lookup(referenceToken->code) == Delimiter::Semicolon) {
        referenceToken = tokens.pop();
    }

    // Clear the node stack until the parent node
    _nodeStack.popUntil([&](TreeNode<SyntaxData>* node) {
        return node == &parentNode;
    });
}

Token Parser::expect(
    const std::string& expected,
    const std::string& errorMessage
) {
    auto token = tokens.pop();

    if (!token) {
        panic(errorMessage);
    }

    auto tokenData = symbols.lookup(token->code);

    if (tokenData != expected) {
        panic(errorMessage, *token);
    }

    return *token;
}

Token Parser::expect(SymbolType expected, const std::string& errorMessage) {
    auto token = tokens.pop();

    if (!token) {
        panic(errorMessage);
    }

    if (symbols.lookupType(token->code) != expected) {
        panic(errorMessage, *token);
    }

    return *token;
}

bool Parser::consider(const std::string& expected) {
    auto token = tokens.peek();

    if (!token) {
        return false;
    }

    if (symbols.lookup(token->code) != expected) {
        return false;
    }

    return true;
}

bool Parser::consider(SymbolType expected) {
    auto token = tokens.peek();

    if (!token) {
        return false;
    }

    if (symbols.lookupType(token->code) != expected) {
        return false;
    }

    return true;
}

void Parser::panic(const std::string& message) {
    _logger.message({.message = message, .row = 0, .column = 0});

    throw std::runtime_error(message);
}

void Parser::panic(const std::string& message, const Token& token) {
    _logger.message({
        .message = message,
        .row = token.row,
        .column = token.column,
    });

    throw std::runtime_error(
        std::format("[{}:{}] {}", token.row + 1, token.column + 1, message)
    );
}

const Tree<SyntaxData>& Parser::tree() const {
    return _tree;
}

TreeNode<SyntaxData>& Parser::grow(const SyntaxData& data) {
    auto parentNode = _nodeStack.peek();

    auto& node = (*parentNode)->grow(data);

    _nodeStack.push(&node);

    return node;
}
