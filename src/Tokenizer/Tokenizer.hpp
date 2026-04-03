#pragma once

#include <unordered_map>
#include <vector>

#include "CharacterAttributes.hpp"
#include "Logger.hpp"
#include "Source.hpp"
#include "SymbolStore.hpp"

struct Token {
    size_t code;
    size_t row;
    size_t column;
};

struct Error {
    std::string message;
    size_t row;
    size_t column;
};

class Tokenizer {
private:
    Source& source;
    SymbolStore& symbols;
    CharacterAttributes& attributes;
    Logger<Error>& _logger;

    char character;
    std::string token;
    size_t code;

    std::vector<Token> _tokens;

    void scanWhitespaces();
    void scanInteger();
    void scanString();
    void scanComment();
    void scanMultiDelimiter();
    void scanDelimiter();
    void scanInvalid();

public:
    Tokenizer(
        Source& source,
        SymbolStore& symbols,
        CharacterAttributes& attributes,
        Logger<Error>& logger
    );

    void scan();

    void addToken(const Token& token);
    void addError(const Error& error);

    [[nodiscard]] const std::vector<Token>& tokens() const;
};
