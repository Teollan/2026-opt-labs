#ifndef Tokenizer_class_HPP
#define Tokenizer_class_HPP

#include <vector>
#include <unordered_map>
#include <Source.interface.hpp>
#include <SymbolTables.class.hpp>

struct Token {
    unsigned int code;
    unsigned int line;
    unsigned int column;
};

class Tokenizer {
public:
    Source& source;
    SymbolTables& tables;

    std::vector<Token> tokens;
    std::vector<std::string> errors;
    std::vector<std::string> comments;

public:
    Tokenizer(Source& source, SymbolTables& tables);

    std::vector<Token> getTokens();
    void addToken(const unsigned int code, unsigned int line, unsigned int column);
    void addComment(const std::string& comment);
    void addError(const std::string& error);
};

#endif