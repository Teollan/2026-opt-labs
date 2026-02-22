#pragma once

#include <vector>
#include <unordered_map>
#include "Source.hpp"
#include "SymbolStore.hpp"
#include "CharacterAttributes.hpp"

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

    char character;
    std::string token;
    size_t code;

    std::vector<Token> _tokens;
    std::vector<Error> _errors;

    void scanWhitespaces();
    void scanInteger();
    void scanString();
    void scanComment();
    void scanDelimiter();
    void scanInvalid();

public:
    Tokenizer(Source& source, SymbolStore& symbols, CharacterAttributes& attributes);

    void scan();

    void addToken(const Token& token);
    void addError(const Error& error);

    const std::vector<Token>& tokens() const;
    const std::vector<Error>& errors() const;
};

