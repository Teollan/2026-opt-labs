#include "Tokenizer.class.hpp"

#include "CharacterAttributes.class.hpp"

Tokenizer::Tokenizer(Source& source, SymbolStore& symbols, CharacterAttributes& attributes) :
    source(source),
    symbols(symbols),
    attributes(attributes) {}

std::vector<Token> Tokenizer::getTokens() {
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
                code = symbols.declareLiteral(token);
                addToken(code, 0, 0);
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
                    code = symbols.declareKeyword(token);
                } else {
                    code = symbols.declareIdentifier(token);
                }
                addToken(code, 0, 0);
                token.clear();

                break;

            // Automata state: BEGIN_COMMENT
            case Attribute::Comment:
                token += character;
                character = source.read();

                // Automata state: DELIMITER_OUT
                if (character != '*') {
                    addToken(token[0], 0, 0);
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
                addToken(static_cast<unsigned int>(character), 0, 0);
                character = source.read();

                break;

            // Automata state: ERROR_INVALID_SYMBOL
            case Attribute::Invalid:
                addError(std::string("Invalid symbol: ") + character);
                character = source.read();
                
                break;
        }
    }

    return tokens;
}

void Tokenizer::addToken(const unsigned int code, unsigned int line, unsigned int column) {
    tokens.push_back({code, line, column});
}

void Tokenizer::addComment(const std::string& comment) {
    comments.push_back(comment);
}

void Tokenizer::addError(const std::string& error) {
    errors.push_back(error);
}