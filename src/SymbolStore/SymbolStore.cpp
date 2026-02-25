#include "SymbolStore.hpp"

#include <algorithm>
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
    if (_keywords.contains(keyword)) {
        throw std::invalid_argument(std::format("Keyword \'{}\' is already declared", keyword));
    }

    size_t code = KEYWORDS_OFFSET + _keywords.size();

    if (code >= LITERALS_OFFSET) {
        throw std::overflow_error("Exceeded maximum number of keywords");
    }

    _keywords[keyword] = code;
    symbols[code] = keyword;
}

size_t SymbolStore::resolveKeyword(const std::string& keyword) {
    if (!_keywords.contains(keyword)) {
        throw std::invalid_argument(std::format("\'{}\' is not a keyword", keyword));
    }

    return _keywords[keyword];
}

size_t SymbolStore::resolveIdentifier(const std::string& identifier) {
    if (!_identifiers.contains(identifier)) {
        size_t code = IDENTIFIERS_OFFSET + _identifiers.size();

        if (code >= MAX_TOKENS) {
            throw std::overflow_error("Exceeded maximum number of identifiers");
        }

        _identifiers[identifier] = code;
        symbols[code] = identifier;
    }

    return _identifiers[identifier];
}

size_t SymbolStore::resolveLiteral(const std::string& literal) {
    if (!_literals.contains(literal)) {
        size_t code = LITERALS_OFFSET + _literals.size();

        if (code >= IDENTIFIERS_OFFSET) {
            throw std::overflow_error("Exceeded maximum number of literals");
        }

        _literals[literal] = code;
        symbols[code] = literal;
    }

    return _literals[literal];
}

bool SymbolStore::isKeyword(const std::string& token) const {
    return _keywords.contains(token);
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

std::vector<std::pair<std::string, size_t>> SymbolStore::identifiers() const {
    std::vector<std::pair<std::string, size_t>> result;

    for (const auto& pair : _identifiers) {
        result.emplace_back(pair.first, pair.second);
    }

    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    return result;
}

std::vector<std::pair<std::string, size_t>> SymbolStore::literals() const {
    std::vector<std::pair<std::string, size_t>> result;

    for (const auto& pair : _literals) {
        result.emplace_back(pair.first, pair.second);
    }

    std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    return result;
}