#include "Tokenizer.class.hpp"

Tokenizer::Tokenizer(Source& source, SymbolTables& tables) :
    source(source),
    tables(tables) {}

std::vector<Token> Tokenizer::getTokens() {
    std::string token;
    unsigned int code;
    Symbol symbol;

    while (!source.isEnd()) {
        symbol = source.getCurrentSymbol();

        switch (symbol.getCategory()) {
            // Automata state: WHITESPACE
            case SymbolCategory::Whitespace:
                source.advance();
                break;

            // Automata state: CONSTANT_IN
            case SymbolCategory::Digit:
                while (!source.isEnd() && symbol.getCategory() == SymbolCategory::Digit) {
                    token += symbol.getCharacter();
                    symbol = source.advance();
                };

                // Automata state: TOKEN_OUT
                code = tables.declareLiteral(token);
                addToken(code, 0, 0);
                token.clear();

                break;

            // Automata state: IDENTIFIER_IN
            case SymbolCategory::Letter:
                do {
                    token += symbol.getCharacter();
                    symbol = source.advance();
                } while (!source.isEnd() && (symbol.getCategory() == SymbolCategory::Letter || symbol.getCategory() == SymbolCategory::Digit));

                // Automata state: TOKEN_OUT
                if (tables.isKeyword(token)) {
                    code = tables.declareKeyword(token);
                } else {
                    code = tables.declareIdentifier(token);
                }
                addToken(code, 0, 0);
                token.clear();

                break;

            // Automata state: BEGIN_COMMENT
            case SymbolCategory::Comment:
                token += symbol.getCharacter();
                symbol = source.advance();

                // Automata state: DELIMITER_OUT
                if (symbol.getCharacter() != '*') {
                    addToken(token[0], 0, 0);
                    token.clear();

                    break;
                }

                // Automata state: COMMENT_IN
                while (!source.isEnd()) {
                    token += symbol.getCharacter();
                    symbol = source.advance();

                    // Automata state: ERROR_COMMENT_NOT_CLOSED
                    if (source.isEnd()) {
                        addError("Comment not closed: " + token);
                        token.clear();

                        break;
                    }

                    // Automata state: COMMENT_END
                    while (!source.isEnd()) {
                        token += symbol.getCharacter();
                        symbol = source.advance();

                        if (symbol.getCharacter() != '*') {
                            break;
                        }
                    }

                    // Automata state: ERROR_COMMENT_NOT_CLOSED
                    if (source.isEnd()) {
                        addError("Comment not closed: " + token);
                        token.clear();

                        break;
                    }

                    // Automata state: COMMENT_OUT
                    if (symbol.getCharacter() == ')') {
                        token += symbol.getCharacter();
                        addComment(token);

                        source.advance();
                        token.clear();

                        break;
                    }
                };

                break;

            case SymbolCategory::SingleDelimiter:
                // Automata state: DELIMITER_OUT
                addToken(static_cast<unsigned int>(symbol.getCharacter()), 0, 0);
                symbol = source.advance();
                break;

            case SymbolCategory::Invalid:
                // Automata state: ERROR_INVALID_SYMBOL
                addError(std::string("Invalid symbol: ") + symbol.getCharacter());
                symbol = source.advance();
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