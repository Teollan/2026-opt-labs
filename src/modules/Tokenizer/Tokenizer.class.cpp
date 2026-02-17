#include "Tokenizer.class.hpp"

#include "CharacterAttributes.class.hpp"

Tokenizer::Tokenizer(Source& source, SymbolStore& symbols, CharacterAttributes& attributes) :
    source(source),
    symbols(symbols),
    attributes(attributes) {}

void Tokenizer::scan() {
    std::string token;
    unsigned int code;
    char character = source.current();

    while (!source.done()) {
        switch (attributes.lookup(character)) {
            // Automata state: WHITESPACE
            case Attribute::Whitespace:
                character = source.read();

                break;

            // Automata state: CONSTANT_IN
            case Attribute::Digit:
                while (!source.done() && attributes.lookup(character) == Attribute::Digit) {
                    token += character;
                    character = source.read();
                };

                // Automata state: TOKEN_OUT
                code = symbols.resolveLiteral(token);

                addToken({
                    .code = code,
                    .row = source.row(),
                    .column = source.column() - static_cast<unsigned int>(token.length()), // Point to first character of the token
                });

                token.clear();

                break;

            // Automata state: IDENTIFIER_IN
            case Attribute::Letter:
                do {
                    token += character;
                    character = source.read();
                } while (!source.done() && (attributes.lookup(character) == Attribute::Letter || attributes.lookup(character) == Attribute::Digit));

                // Automata state: TOKEN_OUT
                if (symbols.isKeyword(token)) {
                    code = symbols.resolveKeyword(token);
                } else {
                    code = symbols.resolveIdentifier(token);
                }

                addToken({
                    .code = code,
                    .row = source.row(),
                    .column = source.column() - static_cast<unsigned int>(token.length()), // Point to first character of the token
                });

                token.clear();

                break;

            // Automata state: BEGIN_COMMENT
            case Attribute::Comment:
                token += character;
                character = source.read();

                // Automata state: DELIMITER_OUT
                if (character != '*') {
                    addToken({
                        .code = static_cast<unsigned int>(token[0]),
                        .row = source.row(),
                        .column = source.column() - 1, // Point to previous character
                    });

                    token.clear();

                    break;
                }

                // Automata state: COMMENT_IN
                while (!source.done()) {
                    token += character;
                    character = source.read();

                    // Automata state: ERROR_COMMENT_NOT_CLOSED
                    if (source.done()) {
                        addError("Comment not closed: " + token);
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

                    // Automata state: ERROR_COMMENT_NOT_CLOSED
                    if (source.done()) {
                        addError("Comment not closed: " + token);
                        token.clear();

                        break;
                    }

                    // Automata state: COMMENT_OUT
                    if (character == ')') {
                        token += character;
                        character = source.read();

                        addComment(token);
                        token.clear();

                        break;
                    }
                };

                break;

            // Automata state: DELIMITER_OUT
            case Attribute::Delimiter:
                addToken({
                    .code = static_cast<unsigned int>(character),
                    .row = source.row(),
                    .column = source.column(),
                });

                character = source.read();

                break;

            // Automata state: ERROR_INVALID_SYMBOL
            case Attribute::Invalid:
                addError(std::string("Invalid symbol: ") + character);
                character = source.read();
                
                break;
        }
    }

}

const std::vector<Token>& Tokenizer::tokens() const {
    return _tokens;
}

const std::vector<std::string>& Tokenizer::errors() const {
    return _errors;
}

const std::vector<std::string>& Tokenizer::comments() const {
    return _comments;
}

void Tokenizer::addToken(const Token& token) {
    _tokens.push_back(token);
}

void Tokenizer::addComment(const std::string& comment) {
    _comments.push_back(comment);
}

void Tokenizer::addError(const std::string& error) {
    _errors.push_back(error);
}