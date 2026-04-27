#pragma once

#include <vector>

#include "CharacterAttributes.hpp"
#include "LexicalError.hpp"
#include "Logger.hpp"
#include "Source.hpp"
#include "SymbolStore.hpp"
#include "Token.hpp"

class Tokenizer {
private:
    Source& _source;
    SymbolStore& _symbols;
    CharacterAttributes& _attributes;
    Logger<LexicalError>& _logger;

    char _character;
    std::string _token;
    size_t _code;

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
        Logger<LexicalError>& logger
    );

    void scan();

    void addToken(const Token& token);
    void addError(const LexicalError& error);

    [[nodiscard]] const std::vector<Token>& tokens() const;
};
