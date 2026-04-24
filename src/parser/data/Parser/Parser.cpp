#include "Parser.hpp"

#include <Rules.hpp>

Parser::Parser(
    const SymbolStore& symbols,
    const std::vector<Token>& tokens,
    Logger<SyntaxError>& logger
) :
    _symbols(symbols),
    _logger(logger),
    _tokens(tokens),
    _tree(Tree<SyntaxData>({.rule = RuleKey::Axiom, .token = _tokens.peek()})) {
    _nodes.push(&_tree.root());
}

void Parser::parse() {
    parseSignalProgram();
}

// 1. <signal-program> --> <program>
void Parser::parseSignalProgram() {
    auto scope =
        grow({.rule = RuleKey::SignalProgram, .token = _tokens.peek()});

    try {
        parseProgram();
    } catch (const SyntaxError&) {
        // If parsing fails at the top level, we can't really recover,
        // so we just clear the remaining tokens to prevent cascading errors.
        _tokens.clear();
    }

    if (!_tokens.isEmpty()) {
        auto token = _tokens.peek();

        _logger.message(SyntaxError(
            SyntaxError::UnexpectedSymbolsAfterEndOfProgram, token->row,
            token->column
        ));
    }
}

// 2. <program> --> PROGRAM <procedure-identifier>; <block>.
void Parser::parseProgram() {
    auto scope = grow({.rule = RuleKey::Program, .token = _tokens.peek()});

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
    auto scope = grow({.rule = RuleKey::Block, .token = _tokens.peek()});

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
    auto scope =
        grow({.rule = RuleKey::StatementsList, .token = _tokens.peek()});
}

// 5. <declarations> --> <constant-declarations>
void Parser::parseDeclarations() {
    auto scope = grow({.rule = RuleKey::Declarations, .token = _tokens.peek()});

    parseConstantDeclarations();
}

// 6. <constant-declarations> --> CONST <constant-declarations-list> | <empty>
void Parser::parseConstantDeclarations() {
    if (consider(Keyword::Const)) {
        auto scope = grow(
            {.rule = RuleKey::ConstantDeclarations, .token = _tokens.peek()}
        );

        expect(Keyword::Const, SyntaxError::ExpectedConstKeyword);
        parseConstantDeclarationsList();
    }

    auto scope = grow(
        {.rule = RuleKey::ConstantDeclarationsEmpty, .token = _tokens.peek()}
    );
}

// 7. <constant-declarations-list> --> <constant-declaration>
// <constant-declarations-list> | <empty>
void Parser::parseConstantDeclarationsList() {
    if (!consider(Keyword::Begin)) {
        auto scope = grow(
            {.rule = RuleKey::ConstantDeclarationsList, .token = _tokens.peek()}
        );

        try {
            parseConstantDeclaration();
        } catch (const SyntaxError&) {
            skipToNextDeclaration();
        }

        parseConstantDeclarationsList();
    } else {
        auto scope = grow(
            {.rule = RuleKey::ConstantDeclarationsListEmpty,
             .token = _tokens.peek()}
        );
    }
}

// 8. <constant-declaration> --> <constant-identifier> = <constant>;
void Parser::parseConstantDeclaration() {
    auto scope =
        grow({.rule = RuleKey::ConstantDeclaration, .token = _tokens.peek()});

    parseConstantIdentifier();
    expect(Delimiter::Equals, SyntaxError::ExpectedEquals);
    parseConstant();
    try {
        expect(Delimiter::Semicolon, SyntaxError::ExpectedConstantSemicolon);
    } catch (const SyntaxError&) {
        // Implicitly add a semicolon to recover and continue parsing the next
        // declarations.
        // TODO: check that this does not backfire for severely mangled
        // declarations.
    }
}

// 9. <constant> --> '<complex-number>'
void Parser::parseConstant() {
    auto scope = grow({.rule = RuleKey::Constant, .token = _tokens.peek()});

    expect(Delimiter::Quote, SyntaxError::ExpectedOpeningQuote);
    parseComplexNumber();
    expect(Delimiter::Quote, SyntaxError::ExpectedClosingQuote);
}

// 10. <complex-number> --> <left-part> <right-part>
void Parser::parseComplexNumber() {
    auto scope =
        grow({.rule = RuleKey::ComplexNumber, .token = _tokens.peek()});

    parseLeftPart();
    parseRightPart();
}

// 11. <left-part> --> <unsigned-integer> | <empty>
void Parser::parseLeftPart() {
    if (consider(SymbolType::Literal)) {
        auto scope =
            grow({.rule = RuleKey::LeftPartValue, .token = _tokens.peek()});

        parseUnsignedInteger();
    } else {
        auto scope =
            grow({.rule = RuleKey::LeftPartEmpty, .token = _tokens.peek()});
    }
}

// 12. <right-part> --> ,<unsigned-integer> | $EXP( <unsigned-integer> ) |
// <empty>
void Parser::parseRightPart() {
    if (consider(Delimiter::Comma)) {
        auto scope =
            grow({.rule = RuleKey::RightPartBase, .token = _tokens.peek()});

        expect(Delimiter::Comma, SyntaxError::ExpectedComma);
        parseUnsignedInteger();
        return;
    }

    if (consider(MultiDelimiter::Exp)) {
        auto scope =
            grow({.rule = RuleKey::RightPartExp, .token = _tokens.peek()});

        expect(MultiDelimiter::Exp, SyntaxError::ExpectedExp);
        expect(Delimiter::OpenParenthesis, SyntaxError::ExpectedOpenParen);
        parseUnsignedInteger();
        expect(Delimiter::CloseParenthesis, SyntaxError::ExpectedCloseParen);
        return;
    }

    auto scope =
        grow({.rule = RuleKey::RightPartEmpty, .token = _tokens.peek()});
}

// 13. <constant-identifier> --> <identifier>
void Parser::parseConstantIdentifier() {
    auto scope =
        grow({.rule = RuleKey::ConstantIdentifier, .token = _tokens.peek()});

    parseIdentifier(SyntaxError::ExpectedConstantIdentifier);
}

// 14. <procedure-identifier> --> <identifier>
void Parser::parseProcedureIdentifier() {
    auto scope =
        grow({.rule = RuleKey::ProcedureIdentifier, .token = _tokens.peek()});

    parseIdentifier(SyntaxError::ExpectedProcedureIdentifier);
}

// 15. <identifier> --> <letter><string>  (terminal)
void Parser::parseIdentifier(const std::string& errorMessage) {
    auto token = expect(SymbolType::Identifier, errorMessage);

    auto scope = grow({.rule = RuleKey::Terminal, .token = token});
}

// 17. <unsigned-integer> --> <digit><digits-string>  (terminal)
void Parser::parseUnsignedInteger() {
    auto token =
        expect(SymbolType::Literal, SyntaxError::ExpectedUnsignedInteger);

    auto scope = grow({.rule = RuleKey::Terminal, .token = token});
}

// --- Error recovery ---

void Parser::skipToProcedureBody() {
    auto found = _tokens.popUntil([&](const Token& token) {
        const auto& symbol = _symbols.lookup(token.code);
        return symbol == Keyword::Const || symbol == Keyword::Begin;
    });

    if (!found) {
        fail(SyntaxError::UnexpectedEndOfFile);
    }
}

void Parser::skipToBlockEnd() {
    auto found = _tokens.popUntil([&](const Token& token) {
        const auto& symbol = _symbols.lookup(token.code);
        return symbol == Keyword::End || symbol == Delimiter::Dot;
    });

    if (!found) {
        fail(SyntaxError::UnexpectedEndOfFile);
    }

    if (_symbols.lookup(found->code) == Keyword::End) {
        _tokens.pop();
    }
}

void Parser::skipToNextDeclaration() {
    auto found = _tokens.popUntil([&](const Token& token) {
        const auto& symbol = _symbols.lookup(token.code);
        return symbol == Delimiter::Semicolon || symbol == Keyword::Begin;
    });

    if (!found) {
        fail(SyntaxError::UnexpectedEndOfFile);
    }

    if (_symbols.lookup(found->code) == Delimiter::Semicolon) {
        _tokens.pop();
    }
}

// --- Utilities ---

Token Parser::expect(
    const std::string& expected,
    const std::string& errorMessage
) {
    auto token = _tokens.peek();

    if (!token) {
        fail(errorMessage);
    }

    if (_symbols.lookup(token->code) != expected) {
        fail(errorMessage, *token);
    }

    return *_tokens.pop();
}

Token Parser::expect(SymbolType expected, const std::string& errorMessage) {
    auto token = _tokens.peek();

    if (!token) {
        fail(errorMessage);
    }

    if (_symbols.lookupType(token->code) != expected) {
        fail(errorMessage, *token);
    }

    return *_tokens.pop();
}

bool Parser::consider(const std::string& expected) {
    auto token = _tokens.peek();

    return token && _symbols.lookup(token->code) == expected;
}

bool Parser::consider(SymbolType expected) {
    auto token = _tokens.peek();

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
    auto parentNode = _nodes.peek();
    auto& node = (*parentNode)->grow(data);
    _nodes.push(&node);
    return ParsingScope(_nodes);
}

const Tree<SyntaxData>& Parser::tree() const {
    return _tree;
}
