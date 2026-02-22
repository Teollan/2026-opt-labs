#include "SymbolStore.hpp"

#include <format>
#include <stdexcept>

SymbolStore::SymbolStore() {
    // Initialize ASCII symbols
    for (size_t i = 0; i < ASCII_TABLE_SIZE; i++) {
        declareDelimiter(static_cast<char>(i));
    }

    // Initialize keywords
    declareKeyword("PROGRAM");
    declareKeyword("CONST");
    declareKeyword("BEGIN");
    declareKeyword("END");
    declareKeyword("EXP");
}

void SymbolStore::declareDelimiter(const char character) {
    symbols[static_cast<unsigned char>(character)] = std::string(1, character);
}

void SymbolStore::declareKeyword(const std::string& keyword) {
    if (keywords.contains(keyword)) {
        throw std::invalid_argument(std::format("Keyword \'{}\' is already declared", keyword));
    }

    size_t code = KEYWORDS_OFFSET + keywords.size();

    if (code >= LITERALS_OFFSET) {
        throw std::overflow_error("Exceeded maximum number of keywords");
    }

    keywords[keyword] = code;
    symbols[code] = keyword;
}

size_t SymbolStore::resolveKeyword(const std::string& keyword) {
    if (!keywords.contains(keyword)) {
        throw std::invalid_argument(std::format("\'{}\' is not a keyword", keyword));
    }

    return keywords[keyword];
}

size_t SymbolStore::resolveIdentifier(const std::string& identifier) {
    if (!identifiers.contains(identifier)) {
        size_t code = IDENTIFIERS_OFFSET + identifiers.size();
        if (code >= MAX_TOKENS) {
            throw std::overflow_error("Exceeded maximum number of identifiers");
        }
        identifiers[identifier] = code;
        symbols[code] = identifier;
    }

    return identifiers[identifier];
}

size_t SymbolStore::resolveLiteral(const std::string& literal) {
    if (!literals.contains(literal)) {
        size_t code = LITERALS_OFFSET + literals.size();
        if (code >= IDENTIFIERS_OFFSET) {
            throw std::overflow_error("Exceeded maximum number of literals");
        }
        literals[literal] = code;
        symbols[code] = literal;
    }

    return literals[literal];
}

bool SymbolStore::isKeyword(const std::string& token) const {
    return keywords.contains(token);
}

SymbolType SymbolStore::lookupType(size_t code) const {
    if (code < KEYWORDS_OFFSET) {
        return SymbolType::Delimiter;
    }
    
    if (code < LITERALS_OFFSET) {
        return SymbolType::Keyword;
    }
    
    if (code < IDENTIFIERS_OFFSET) {
        return SymbolType::Literal;
    }

    if (code < MAX_TOKENS) {
        return SymbolType::Identifier;
    }

    throw std::out_of_range(std::format("Symbol code {} is out of range", code));
}

const std::string& SymbolStore::lookup(size_t code) const {
    if (code >= symbols.size()) {
        throw std::out_of_range(std::format("Symbol code {} is out of range", code));
    }

    return symbols[code];
}