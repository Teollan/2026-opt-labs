#include "Parser.hpp"

#include <format>

Parser::Parser(
    const SymbolStore& symbols,
    const std::vector<Token>& tokens,
    Logger<SyntaxError>& logger
) :
    _symbols(symbols),
    _logger(logger),
    _tokenStack(Stack<Token>({tokens.rbegin(), tokens.rend()})),
    _tree(Tree<SyntaxData>({.symbol = "<signal-program>", .rule = "S"})) {
    _nodeStack.push(&_tree.root());
}

void Parser::parse() {
    parseSignalProgram();
}

// 1. <signal-program> --> <program>
void Parser::parseSignalProgram() {
    auto scope = grow({.symbol = "<program>", .rule = "1"});

    try {
        parseProgram();
    } catch (const SyntaxError&) {
        // If parsing fails at the top level, we can't really recover, so we just clear the remaining tokens to prevent cascading errors.
        _tokenStack.clear();
    }

    if (!_tokenStack.isEmpty()) {
        auto token = _tokenStack.peek();
        
        _logger.message(SyntaxError(
            SyntaxError::UnexpectedSymbolsAfterEndOfProgram,
            token->row,
            token->column
        ));
    }
}

// 2. <program> --> PROGRAM <procedure-identifier>; <block>.
void Parser::parseProgram() {
    auto scope = grow({
        .symbol = "PROGRAM <procedure-identifier>; <block>.",
        .rule = "2",
    });

    try {
        expect(Keyword::Program, SyntaxError::MustStartWithProgram);
        parseProcedureIdentifier();
        expect(Delimiter::Semicolon, SyntaxError::ExpectedSemicolon);
    } catch (const SyntaxError&) {
        skipToProcedureBody();
    }

    try {
        parseBlock();
    } catch (const SyntaxError&) {
        skipToBlockEnd();
    }

    expect(Delimiter::Dot, SyntaxError::ExpectedDot);
}

// 3. <block> --> <declarations> BEGIN <statements-list> END
void Parser::parseBlock() {
    auto scope = grow({
        .symbol = "<declarations> BEGIN <statements-list> END",
        .rule = "3",
    });

    try {
        parseDeclarations();
    } catch (const SyntaxError& error) {
        if (error.message == SyntaxError::ExpectedConstKeyword) {
            skipToProcedureBody();
        } else {
            throw error;
        }
    }
    
    expect(Keyword::Begin, SyntaxError::ExpectedBeginKeyword);
    parseStatementsList();
    expect(Keyword::End, SyntaxError::ExpectedEndKeyword);
}

// 4. <statements-list> --> <empty>
void Parser::parseStatementsList() {
    auto scope = grow({
        .symbol = "<empty>",
        .rule = "4",
    });
}

// 5. <declarations> --> <constant-declarations>
void Parser::parseDeclarations() {
    auto scope = grow({
        .symbol = "<constant-declarations>",
        .rule = "5",
    });

    parseConstantDeclarations();
}

// 6. <constant-declarations> --> CONST <constant-declarations-list> | <empty>
void Parser::parseConstantDeclarations() {
    if (consider(Keyword::Const)) {
        auto scope = grow({
            .symbol = "CONST <constant-declarations-list>",
            .rule = "6",
        });

        expect(Keyword::Const, SyntaxError::ExpectedConstKeyword);
        parseConstantDeclarationsList();
    } 
    
    auto scope = grow({
        .symbol = "<empty>",
        .rule = "6",
    });
}

// 7. <constant-declarations-list> --> <constant-declaration>
// <constant-declarations-list> | <empty>
void Parser::parseConstantDeclarationsList() {
    if (!consider(Keyword::Begin)) {
        auto scope = grow({
            .symbol = "<constant-declaration> <constant-declarations-list>",
            .rule = "7",
        });

        try {
            parseConstantDeclaration();
        } catch (const SyntaxError&) {
            skipToNextDeclaration();
        }

        parseConstantDeclarationsList();
    } else {
        auto scope = grow({.symbol = "<empty>", .rule = "7"});
    }
}

// 8. <constant-declaration> --> <constant-identifier> = <constant>;
void Parser::parseConstantDeclaration() {
    auto scope = grow({
        .symbol = "<constant-identifier> = <constant>;",
        .rule = "8",
    });

    parseConstantIdentifier();
    expect(Delimiter::Equals, SyntaxError::ExpectedEquals);
    parseConstant();
    try {
        expect(Delimiter::Semicolon, SyntaxError::ExpectedConstantSemicolon);
    } catch (const SyntaxError&) {
        // Implicitly add a semicolon to recover and continue parsing the next declarations.
        // TODO: check that this does not backfire for severely mangled declarations.
    }
}

// 9. <constant> --> '<complex-number>'
void Parser::parseConstant() {
    auto scope = grow({
        .symbol = "'<complex-number>'",
        .rule = "9",
    });

    expect(Delimiter::Quote, SyntaxError::ExpectedOpeningQuote);
    parseComplexNumber();
    expect(Delimiter::Quote, SyntaxError::ExpectedClosingQuote);
}

// 10. <complex-number> --> <left-part> <right-part>
void Parser::parseComplexNumber() {
    auto scope = grow({
        .symbol = "<left-part> <right-part>",
        .rule = "10",
    });

    parseLeftPart();
    parseRightPart();
}

// 11. <left-part> --> <unsigned-integer> | <empty>
void Parser::parseLeftPart() {
    if (consider(SymbolType::Literal)) {
        auto scope = grow({
            .symbol = "<unsigned-integer>",
            .rule = "11",
        });

        parseUnsignedInteger();
    } else {
        auto scope = grow({.symbol = "<empty>", .rule = "11"});
    }
}

// 12. <right-part> --> ,<unsigned-integer> | $EXP( <unsigned-integer> ) |
// <empty>
void Parser::parseRightPart() {
    if (consider(Delimiter::Comma)) {
        auto scope = grow({
            .symbol = ",<unsigned-integer>",
            .rule = "12",
        });

        expect(Delimiter::Comma, SyntaxError::ExpectedComma);
        parseUnsignedInteger();
        return;
    }

    if (consider(MultiDelimiter::Exp)) {
        auto scope = grow({
            .symbol = "$EXP( <unsigned-integer> )",
            .rule = "12",
        });

        expect(MultiDelimiter::Exp, SyntaxError::ExpectedExp);
        expect(Delimiter::OpenParenthesis, SyntaxError::ExpectedOpenParen);
        parseUnsignedInteger();
        expect(Delimiter::CloseParenthesis, SyntaxError::ExpectedCloseParen);
        return;
    }

    auto scope = grow({.symbol = "<empty>", .rule = "12"});
}

// 13. <constant-identifier> --> <identifier>
void Parser::parseConstantIdentifier() {
    auto scope = grow({
        .symbol = "<identifier>",
        .rule = "13"
    });

    parseIdentifier(SyntaxError::ExpectedConstantIdentifier);
}

// 14. <procedure-identifier> --> <identifier>
void Parser::parseProcedureIdentifier() {
    auto scope = grow({.symbol = "<identifier>", .rule = "14"});

    parseIdentifier(SyntaxError::ExpectedProcedureIdentifier);
}

// 15. <identifier> --> <letter><string>
void Parser::parseIdentifier(const std::string& errorMessage) {
    auto token = expect(SymbolType::Identifier, errorMessage);

    auto scope = grow({
        .symbol = _symbols.lookup(token.code),
        .rule = "15",
    });
}

// 17. <unsigned-integer> --> <digit><digits-string>
void Parser::parseUnsignedInteger() {
    auto token = expect(SymbolType::Literal, SyntaxError::ExpectedUnsignedInteger);

    auto scope = grow({
        .symbol = _symbols.lookup(token.code),
        .rule = "17",
    });
}

// --- Error recovery ---

void Parser::skipToProcedureBody() {
    auto found = _tokenStack.popUntil([&](const Token& token) {
        const auto& symbol = _symbols.lookup(token.code);
        return symbol == Keyword::Const || symbol == Keyword::Begin;
    });

    if (!found) {
        fail(SyntaxError::UnexpectedEndOfFile);
    }
}

void Parser::skipToBlockEnd() {
    auto found = _tokenStack.popUntil([&](const Token& token) {
        const auto& symbol = _symbols.lookup(token.code);
        return symbol == Keyword::End || symbol == Delimiter::Dot;
    });

    if (!found) {
        fail(SyntaxError::UnexpectedEndOfFile);
    }

    if (_symbols.lookup(found->code) == Keyword::End) {
        _tokenStack.pop();
    }
}

void Parser::skipToNextDeclaration() {
    auto found = _tokenStack.popUntil([&](const Token& token) {
        const auto& symbol = _symbols.lookup(token.code);
        return symbol == Delimiter::Semicolon || symbol == Keyword::Begin;
    });

    if (!found) {
        fail(SyntaxError::UnexpectedEndOfFile);
    }

    if (_symbols.lookup(found->code) == Delimiter::Semicolon) {
        _tokenStack.pop();
    }
}

// --- Utilities ---

Token Parser::expect(
    const std::string& expected,
    const std::string& errorMessage
) {
    auto token = _tokenStack.peek();

    if (!token) {
        fail(errorMessage);
    }

    if (_symbols.lookup(token->code) != expected) {
        fail(errorMessage, *token);
    }

    return *_tokenStack.pop();
}

Token Parser::expect(SymbolType expected, const std::string& errorMessage) {
    auto token = _tokenStack.peek();

    if (!token) {
        fail(errorMessage);
    }

    if (_symbols.lookupType(token->code) != expected) {
        fail(errorMessage, *token);
    }

    return *_tokenStack.pop();
}

bool Parser::consider(const std::string& expected) {
    auto token = _tokenStack.peek();

    return token && _symbols.lookup(token->code) == expected;
}

bool Parser::consider(SymbolType expected) {
    auto token = _tokenStack.peek();

    return token && _symbols.lookupType(token->code) == expected;
}

void Parser::fail(const std::string& message) {
    SyntaxError error(message, 0, 0);
    _logger.message(error);
    throw error;
}

void Parser::fail(const std::string& message, const Token& token) {
    SyntaxError error(message, token.row, token.column);
    _logger.message(error);
    throw error;
}

ParsingScope Parser::grow(const SyntaxData& data) {
    auto parentNode = _nodeStack.peek();
    auto& node = (*parentNode)->grow(data);
    _nodeStack.push(&node);
    return ParsingScope(_nodeStack);
}

const Tree<SyntaxData>& Parser::tree() const {
    return _tree;
}
