#include "Tokenizer.hpp"

#include <format>
#include "CharacterAttributes.hpp"

Tokenizer::Tokenizer(Source& source, SymbolStore& symbols, CharacterAttributes& attributes) :
    source(source),
    symbols(symbols),
    attributes(attributes),
    character(source.current()) {}

void Tokenizer::scan() {
    while (!source.done()) {
        switch (attributes.lookup(character)) {
            // Automata state: WHITESPACE
            case Attribute::Whitespace:
                scanWhitespaces();
                break;

            // Automata state: INTEGER_IN
            case Attribute::Digit:
                scanInteger();
                break;

            // Automata state: STRING_IN
            case Attribute::Letter:
                scanString();
                break;

            // Automata state: BEGIN_COMMENT
            case Attribute::Comment:
                scanComment();
                break;

            // Automata state: TOKEN_OUT
            case Attribute::Delimiter:
                scanDelimiter();
                break;

            // Automata state: ERROR
            case Attribute::Invalid:
                scanInvalid();
                break;
        }
    }

}

void Tokenizer::scanWhitespaces() {
    while (!source.done() && attributes.lookup(character) == Attribute::Whitespace) {
        character = source.read();
    }
}

void Tokenizer::scanInteger() {
    while (!source.done() && attributes.lookup(character) == Attribute::Digit) {
        token += character;
        character = source.read();
    };

    // Automata state: TOKEN_OUT
    code = symbols.resolveLiteral(token);

    addToken({
        .code = code,
        .row = source.row(),
        .column = source.column() - token.length(), // Point to first character of the token
    });

    token.clear();
}

void Tokenizer::scanString() {
    while (!source.done() && (attributes.lookup(character) == Attribute::Letter || attributes.lookup(character) == Attribute::Digit)) {
        token += character;
        character = source.read();
    }

    // Automata state: TOKEN_OUT
    if (symbols.isKeyword(token)) {
        code = symbols.resolveKeyword(token);
    } else {
        code = symbols.resolveIdentifier(token);
    }

    addToken({
        .code = code,
        .row = source.row(),
        .column = source.column() - token.length(), // Point to first character of the token
    });

    token.clear();
}

void Tokenizer::scanComment() {
    token += character;
    character = source.read();

    // Automata state: TOKEN_OUT
    if (character != '*') {
        addToken({
            .code = static_cast<size_t>(token[0]),
            .row = source.row(),
            .column = source.column() - 1,
        });

        token.clear();

        return;
    }

    // Automata state: COMMENT_IN
    while (!source.done()) {
        token += character;
        character = source.read();

        // Automata state: ERROR_COMMENT_NOT_CLOSED
        if (source.done()) {
            addError({
                .message = std::format("Comment not closed: {}", token),
                .row = source.row(),
                .column = source.column() - token.length(),
            });

            token.clear();

            break;
        }

        // Automata state: COMMENT_END
        while (!source.done()) {
            token += character;
            character = source.read();

            if (character != '*') {
                break;
            }
        }

        // Automata state: ERROR
        if (source.done()) {
            addError({
                .message = std::format("Comment not closed: {}", token),
                .row = source.row(),
                .column = source.column() - token.length(),
            });

            token.clear();

            break;
        }

        // Automata state: COMMENT_OUT
        if (character == ')') {
            token += character;
            character = source.read();

            token.clear();

            break;
        }
    }
}

void Tokenizer::scanDelimiter() {
    addToken({
        .code = static_cast<size_t>(character),
        .row = source.row(),
        .column = source.column(),
    });

    character = source.read();
}

void Tokenizer::scanInvalid() {
    addError({
        .message = std::format("Invalid character: \'{}\'", character),
        .row = source.row(),
        .column = source.column(),
    });

    character = source.read();
}

const std::vector<Token>& Tokenizer::tokens() const {
    return _tokens;
}

const std::vector<Error>& Tokenizer::errors() const {
    return _errors;
}

void Tokenizer::addToken(const Token& token) {
    _tokens.push_back(token);
}

void Tokenizer::addError(const Error& error) {
    _errors.push_back(error);
}