#include "Tokenizer.hpp"

#include <format>

#include "CharacterAttributes.hpp"

// Automata state: START
Tokenizer::Tokenizer(
    Source& source,
    SymbolStore& symbols,
    CharacterAttributes& attributes,
    Logger<Error>& logger
) :
    source(source),
    symbols(symbols),
    attributes(attributes),
    _logger(logger),
    character(source.current()) {}

void Tokenizer::scan() {
    // Automata state: LOOP
    while (!source.done()) {
        switch (attributes.lookup(character)) {
            // Automata state: WHITESPACE
            case Attribute::Whitespace:
                scanWhitespaces();
                break;

            // Automata state: INTEGER
            case Attribute::Digit:
                scanInteger();
                break;

            // Automata state: STRING
            case Attribute::Letter:
                scanString();
                break;

            // Automata state: MULTI_DELIMITER
            case Attribute::MultiDelimiter:
                scanMultiDelimiter();
                break;

            // Automata state: COMMENT_START
            case Attribute::Comment:
                scanComment();
                break;

            // Automata state: DELIMITER
            case Attribute::Delimiter:
                scanDelimiter();
                break;

            // Automata state: ERROR
            case Attribute::Invalid:
                scanInvalid();
                break;
        }
    }
    // Automata state: END
}

void Tokenizer::scanWhitespaces() {
    while (!source.done() &&
           attributes.lookup(character) == Attribute::Whitespace) {
        character = source.read();
    }
}

void Tokenizer::scanInteger() {
    while (!source.done() && attributes.lookup(character) == Attribute::Digit) {
        token += character;
        character = source.read();
    };

    // Automata state: WRITE
    code = symbols.resolveLiteral(token);

    addToken({
        .code = code,
        .row = source.cursor().row(),
        .column = source.cursor().column() -
                  token.length(),  // Point to first character of the token
    });

    token.clear();
}

void Tokenizer::scanString() {
    while (!source.done() &&
           (attributes.lookup(character) == Attribute::Letter ||
            attributes.lookup(character) == Attribute::Digit)) {
        token += character;
        character = source.read();
    }

    // Automata state: WRITE
    if (symbols.isKeyword(token)) {
        code = symbols.resolveKeyword(token);
    } else {
        code = symbols.resolveIdentifier(token);
    }

    addToken({
        .code = code,
        .row = source.cursor().row(),
        .column = source.cursor().column() -
                  token.length(),  // Point to first character of the token
    });

    token.clear();
}

void Tokenizer::scanMultiDelimiter() {
    size_t startRow = source.cursor().row();
    size_t startCol = source.cursor().column();

    token += character;
    character = source.read();

    if (source.done() || attributes.lookup(character) != Attribute::Letter) {
        addError({
            .message = std::format("Invalid use of '$'"),
            .row = startRow,
            .column = startCol,
        });

        token.clear();

        return;
    }

    while (!source.done() &&
           (attributes.lookup(character) == Attribute::Letter)) {
        token += character;
        character = source.read();
    }

    if (!symbols.isMultiDelimiter(token)) {
        addError({
            .message = std::format("Invalid use of '$'"),
            .row = startRow,
            .column = startCol,
        });

        token.clear();
        return;
    }

    code = symbols.resolveMultiDelimiter(token);

    // Automata state: WRITE
    addToken({
        .code = code,
        .row = startRow,
        .column = startCol,
    });

    token.clear();
}

void Tokenizer::scanComment() {
    size_t startRow = source.cursor().row();
    size_t startCol = source.cursor().column();

    character = source.read();

    // Automata state: WRITE
    if (character != '*') {
        addToken({
            .code = static_cast<size_t>('('),
            .row = startRow,
            .column = startCol,
        });

        return;
    }

    character = source.read();

    // Automata state: COMMENT
    while (!source.done()) {
        // Automata state: COMMENT_CLOSE
        if (character == '*') {
            character = source.read();

            // Automata state: COMMENT_END
            if (character == ')') {
                character = source.read();

                return;
            }
        } else {
            character = source.read();
        }
    }

    // Automata state: ERROR
    addError({
        .message = "Comment not closed",
        .row = startRow,
        .column = startCol,
    });
}

void Tokenizer::scanDelimiter() {
    addToken({
        .code = static_cast<size_t>(character),
        .row = source.cursor().row(),
        .column = source.cursor().column(),
    });

    character = source.read();
}

void Tokenizer::scanInvalid() {
    addError({
        .message = std::format("Invalid character \'{}\'", character),
        .row = source.cursor().row(),
        .column = source.cursor().column(),
    });

    character = source.read();
}

const std::vector<Token>& Tokenizer::tokens() const {
    return _tokens;
}

void Tokenizer::addToken(const Token& token) {
    _tokens.push_back(token);
}

void Tokenizer::addError(const Error& error) {
    _logger.message(error);
}