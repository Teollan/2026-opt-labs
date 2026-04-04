#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include "symbols.constants.hpp"

enum class SymbolType {
    Ascii,
    MultiDelimiter,
    Keyword,
    Literal,
    Identifier,
};

const size_t DELIMITERS_OFFSET = 0;
const size_t MULTI_DELIMITERS_OFFSET = ASCII_TABLE_SIZE;
const size_t KEYWORDS_OFFSET = 300;
const size_t LITERALS_OFFSET = 500;
const size_t IDENTIFIERS_OFFSET = 1000;
const size_t MAX_TOKENS = 2000;

class SymbolStore {
private:
    std::array<std::string, MAX_TOKENS> symbols;
    std::unordered_map<std::string, size_t> _multiDelimiters;
    std::unordered_map<std::string, size_t> _keywords;
    std::unordered_map<std::string, size_t> _identifiers;
    std::unordered_map<std::string, size_t> _literals;

    void declareAscii(char character);
    void declareMultiDelimiter(const std::string& delimiter);
    void declareKeyword(const std::string& keyword);

public:
    SymbolStore();

    size_t resolveMultiDelimiter(const std::string& delimiter);
    size_t resolveKeyword(const std::string& keyword);
    size_t resolveIdentifier(const std::string& identifier);
    size_t resolveLiteral(const std::string& literal);

    bool isKeyword(const std::string& token) const;
    bool isMultiDelimiter(const std::string& token) const;

    const std::string& lookup(size_t code) const;
    SymbolType lookupType(size_t code) const;
    std::vector<std::pair<std::string, size_t>> identifiers() const;
    std::vector<std::pair<std::string, size_t>> literals() const;
};
