#include "SymbolStore.class.hpp"

SymbolStore::SymbolStore() {
    // Initialize ASCII symbols
    for (int i = 0; i < 256; i++) {
        symbols[ASCII_OFFSET + i] = std::string(1, static_cast<char>(i));
    }

    // Initialize keywords
    declareKeyword("PROGRAM");
    declareKeyword("CONST");
    declareKeyword("BEGIN");
    declareKeyword("END");
    declareKeyword("EXP");
}

size_t SymbolStore::declareKeyword(const std::string& keyword) {
    if (!keywords.contains(keyword)) {
        size_t code = KEYWORDS_OFFSET + keywords.size();
        keywords[keyword] = code;
        symbols[code] = keyword;
    }

    return keywords[keyword];
}

size_t SymbolStore::declareIdentifier(const std::string& identifier) {
    if (!identifiers.contains(identifier)) {
        size_t code = IDENTIFIERS_OFFSET + identifiers.size();
        identifiers[identifier] = code;
        symbols[code] = identifier;
    }

    return identifiers[identifier];
}

size_t SymbolStore::declareLiteral(const std::string& literal) {
    if (!literals.contains(literal)) {
        size_t code = LITERALS_OFFSET + literals.size();
        literals[literal] = code;
        symbols[code] = literal;
    }

    return literals[literal];
}

bool SymbolStore::isKeyword(const std::string& token) const {
    return keywords.contains(token);
}

SymbolType SymbolStore::determineSymbolType(size_t code) {
    if (code >= ASCII_OFFSET && code < KEYWORDS_OFFSET) {
        return SymbolType::Delimiter;
    }

    if (code >= KEYWORDS_OFFSET && code < LITERALS_OFFSET) {
        return SymbolType::Keyword;
    }

    if (code >= LITERALS_OFFSET && code < IDENTIFIERS_OFFSET) {
        return SymbolType::Literal;
    }

    if (code >= IDENTIFIERS_OFFSET && code < MAX_SYMBOLS) {
        return SymbolType::Identifier;
    }

    throw std::out_of_range("Symbol code out of range");
}

const std::string& SymbolStore::lookup(size_t code) const {
    if (code < ASCII_OFFSET || code >= MAX_SYMBOLS) {
        throw std::out_of_range("Symbol code out of range");
    }

    return symbols[code];
}