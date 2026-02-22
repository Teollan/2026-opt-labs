#pragma once

#include <unordered_map>
#include <string>
#include <vector>

enum class SymbolType {
    Character,
    Keyword,
    Literal,
    Identifier,
};

class SymbolStore {
private:
    std::vector<std::string> symbols;
    std::unordered_map<std::string, size_t> keywords;
    std::unordered_map<std::string, size_t> identifiers;
    std::unordered_map<std::string, size_t> literals;

    void declareCharacter(const char character);
    void declareKeyword(const std::string& keyword);

public:
    SymbolStore();

    size_t resolveKeyword(const std::string& keyword);
    size_t resolveIdentifier(const std::string& identifier);
    size_t resolveLiteral(const std::string& literal);

    bool isKeyword(const std::string& token) const;

    const std::string& lookup(size_t code) const;
    SymbolType lookupType(size_t code) const;
};

