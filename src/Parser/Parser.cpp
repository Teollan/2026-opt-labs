#include "Parser.hpp"

#include <stdexcept>
#include <format>

Parser::Parser(const SymbolStore& symbols, const std::vector<Token>& tokens) :
    symbols(symbols),
    tokens(Stack<Token>({tokens.rbegin(), tokens.rend()})),
    _tree(Tree<std::string>("<signal-program>")) {
    _nodeStack.push(&_tree.root());
}

void Parser::parse() {
    parseProgram();
}

void Parser::parseProgram() {
    grow("<program>");

    expect(Keyword::Program, SyntaxError::MustStartWithProgram);
    expect(SymbolType::Identifier, SyntaxError::ExpectedProcedureIdentifier);
    expect(Delimiter::Semicolon, SyntaxError::ExpectedSemicolon);
    parseBlock();
    expect(Delimiter::Dot, SyntaxError::ExpectedDot);

    _nodeStack.pop();
}

void Parser::parseBlock() {
    grow("<block>");

    parseConstantDeclarations();
    expect(Keyword::Begin, SyntaxError::ExpectedBeginKeyword);
    parseStatementsList();
    expect(Keyword::End, SyntaxError::ExpectedEndKeyword);

    _nodeStack.pop();
}

void Parser::parseConstantDeclarations() {
    grow("<constant-declarations>");

    auto isProvided = consider(Keyword::Const);

    if (isProvided) {
        expect(Keyword::Const, SyntaxError::ExpectedConstKeyword);
        parseConstantDeclarationsList();
    }

    _nodeStack.pop();
}

void Parser::parseStatementsList() {
    // statement list is always empty
    return;
}

void Parser::parseConstantDeclarationsList() {
    grow("<constant-declarations-list>");

    auto isEmpty = consider(Keyword::Begin);

    if (!isEmpty) {
        parseConstantDeclaration();
        parseConstantDeclarationsList();
    }

    _nodeStack.pop();
}

void Parser::parseConstantDeclaration() {
    grow("<constant-declaration>");

    expect(SymbolType::Identifier, SyntaxError::ExpectedIdentifier);
    expect(Delimiter::Equals, SyntaxError::ExpectedEquals);
    parseConstant();
    expect(Delimiter::Semicolon, SyntaxError::ExpectedConstantSemicolon);

    _nodeStack.pop();
}

void Parser::parseConstant() {
    grow("<constant>");

    expect(Delimiter::Quote, SyntaxError::ExpectedOpeningQuote);
    parseComplexNumber();
    expect(Delimiter::Quote, SyntaxError::ExpectedClosingQuote);

    _nodeStack.pop();
}

void Parser::parseComplexNumber() {
    grow("<complex-number>");

    parseLeftPart();
    parseRightPart();

    _nodeStack.pop();
}

void Parser::parseLeftPart() {
    grow("<left-part>");

    auto isProvided = consider(SymbolType::Literal);

    if (isProvided) {
        expect(SymbolType::Literal, SyntaxError::ExpectedUnsignedInteger);
    }

    _nodeStack.pop();
}

void Parser::parseRightPart() {
    grow("<right-part>");

    auto isCommaVariant = consider(Delimiter::Comma);

    if (isCommaVariant) {
        expect(Delimiter::Comma, SyntaxError::ExpectedComma);
        expect(SymbolType::Literal, SyntaxError::ExpectedUnsignedInteger);

        _nodeStack.pop();
        return;
    }

    auto isExpVariant = consider(MultiDelimiter::Exp);

    if (isExpVariant) {
        expect(MultiDelimiter::Exp, SyntaxError::ExpectedExp);
        expect(Delimiter::OpenParenthesis, SyntaxError::ExpectedOpenParen);
        expect(SymbolType::Literal, SyntaxError::ExpectedUnsignedInteger);
        expect(Delimiter::CloseParenthesis, SyntaxError::ExpectedCloseParen);

        _nodeStack.pop();
        return;
    }

    _nodeStack.pop();
}

void Parser::expect(const std::string& expected, const std::string& errorMessage) {
    auto token = tokens.pop();

    auto tokenData = symbols.lookup(token->code);

    grow(tokenData);

    if (!token) {
        panic(errorMessage);
    }

    if (symbols.lookup(token->code) != expected) {
        panic(errorMessage, *token);
    }

    _nodeStack.pop();
}

void Parser::expect(SymbolType expected, const std::string& errorMessage) {
    auto token = tokens.pop();

    grow(symbols.lookup(token->code));

    if (!token) {
        panic(errorMessage);
    }

    if (symbols.lookupType(token->code) != expected) {
        panic(errorMessage, *token);
    }

    _nodeStack.pop();
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
    throw std::runtime_error(message);
}

void Parser::panic(const std::string& message, const Token& token) {
    throw std::runtime_error(
        std::format("Error [{}:{}]: {}", token.row + 1, token.column + 1, message)
    );
}

void Parser::grow(const std::string& data) {
    auto parentNode = _nodeStack.peek();

    auto& node = (*parentNode)->grow(data);

    _nodeStack.push(&node);
}