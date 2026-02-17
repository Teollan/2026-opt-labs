#ifndef Tokenizer_class_HPP
#define Tokenizer_class_HPP

#include <vector>
#include <unordered_map>
#include "Source.class.hpp"
#include "SymbolStore.class.hpp"
#include "CharacterAttributes.class.hpp"

struct Token {
    unsigned int code;
    unsigned int row;
    unsigned int column;
};

class Tokenizer {
private:
    Source& source;
    SymbolStore& symbols;
    CharacterAttributes attributes;

    std::vector<Token> _tokens;
    std::vector<std::string> _errors;
    std::vector<std::string> _comments;

public:
    Tokenizer(Source& source, SymbolStore& symbols, CharacterAttributes& attributes);

    void scan();

    void addToken(const Token& token);
    void addComment(const std::string& comment);
    void addError(const std::string& error);

    const std::vector<Token>& tokens() const;
    const std::vector<std::string>& errors() const;
    const std::vector<std::string>& comments() const;
};

#endif