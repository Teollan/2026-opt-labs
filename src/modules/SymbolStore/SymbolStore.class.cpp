#include "SymbolStore.class.hpp"

#include <format>
#include <stdexcept>

SymbolStore::SymbolStore() {
    // Reserve space for symbols to avoid reallocations
    symbols.reserve(512);

    // Initialize ASCII symbols
    for (int i = 0; i < 256; i++) {
        declareCharacter(static_cast<char>(i));
    }

    // Initialize keywords
    declareKeyword("PROGRAM");
    declareKeyword("CONST");
    declareKeyword("BEGIN");
    declareKeyword("END");
    declareKeyword("EXP");
}

void SymbolStore::declareCharacter(const char character) {
    symbols.emplace_back(std::string(1, character));
}

void SymbolStore::declareKeyword(const std::string& keyword) {
    if (keywords.contains(keyword)) {
        throw std::invalid_argument(std::format("Keyword \'{}\' is already declared", keyword));
    }

    keywords[keyword] = symbols.size();
    symbols.push_back(keyword);
}

size_t SymbolStore::resolveKeyword(const std::string& keyword) {
    if (!keywords.contains(keyword)) {
        throw std::invalid_argument(std::format("\'{}\' is not a keyword", keyword));
    }

    return keywords[keyword];
}

size_t SymbolStore::resolveIdentifier(const std::string& identifier) {
    if (!identifiers.contains(identifier)) {
        identifiers[identifier] = symbols.size();
        symbols.push_back(identifier);
    }

    return identifiers[identifier];
}

size_t SymbolStore::resolveLiteral(const std::string& literal) {
    if (!literals.contains(literal)) {
        literals[literal] = symbols.size();
        symbols.push_back(literal);
    }

    return literals[literal];
}

bool SymbolStore::isKeyword(const std::string& token) const {
    return keywords.contains(token);
}

SymbolType SymbolStore::lookupType(size_t code) const {
    const std::string& token = lookup(code);

    if (code < 256) {
        return SymbolType::Character;
    }

    if (keywords.contains(token)) {
        return SymbolType::Keyword;
    }

    if (identifiers.contains(token)) {
        return SymbolType::Identifier;
    }

    if (literals.contains(token)) {
        return SymbolType::Literal;
    }

    throw std::out_of_range(std::format("Symbol code {} is out of range", code));
}

const std::string& SymbolStore::lookup(size_t code) const {
    if (code >= symbols.size()) {
        throw std::out_of_range(std::format("Symbol code {} is out of range", code));
    }

    return symbols[code];
}