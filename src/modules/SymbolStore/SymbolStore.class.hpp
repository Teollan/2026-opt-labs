#ifndef SYMBOL_STORE_CLASS_HPP
#define SYMBOL_STORE_CLASS_HPP

#include <array>
#include <unordered_map>
#include <string>

const size_t MAX_SYMBOLS = 5000;
const size_t ASCII_OFFSET = 0; // 0 - 255 for ASCII characters like delimiters
const size_t KEYWORDS_OFFSET = 256; // 256 - 999 for keywords
const size_t LITERALS_OFFSET = 1000; // 1000 - 1999 for literals (constants) 
const size_t IDENTIFIERS_OFFSET = 2000; // 2000 - 2999 for identifiers

enum class SymbolType {
    Delimiter,
    Keyword,
    Literal,
    Identifier,
};

class SymbolStore {
private:
    std::array<std::string, MAX_SYMBOLS> symbols;
    std::unordered_map<std::string, size_t> keywords;
    std::unordered_map<std::string, size_t> identifiers;
    std::unordered_map<std::string, size_t> literals;

public:
    SymbolStore();

    static SymbolType determineSymbolType(size_t code);

    size_t declareKeyword(const std::string& keyword);
    size_t declareIdentifier(const std::string& identifier);
    size_t declareLiteral(const std::string& literal);

    bool isKeyword(const std::string& token) const;

    const std::string& lookup(size_t code) const;
};

#endif