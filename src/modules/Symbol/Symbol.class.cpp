#include "Symbol.class.hpp"

#include <array>

constexpr std::array<SymbolCategory, 128> buildCategoryTable() {
    std::array<SymbolCategory, 128> table{};
    for (auto& entry : table) {
        entry = Invalid;
    }

    // 0: Whitespace
    table[' '] = Whitespace;
    table['\t'] = Whitespace;
    table['\n'] = Whitespace;
    table['\r'] = Whitespace;
    table['\v'] = Whitespace;

    // 1: Constant (digits)
    for (char ch = '0'; ch <= '9'; ++ch) {
        table[ch] = Digit;
    }

    // 2: Letters (A-Z, a-z, $ for $EXP keyword)
    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        table[ch] = Letter;
    }
    for (char ch = 'a'; ch <= 'z'; ++ch) {
        table[ch] = Letter;
    }
    // 3: Single-character delimiters
    table[';'] = SingleDelimiter;
    table['.'] = SingleDelimiter;
    table['='] = SingleDelimiter;
    table['\''] = SingleDelimiter;
    table[','] = SingleDelimiter;
    table[')'] = SingleDelimiter;
    table['$'] = SingleDelimiter;

    // 4: Comment start
    table['('] = Comment;

    return table;
}

const auto SYMBOL_CATEGORIES = buildCategoryTable();

Symbol::Symbol(char character): character(character) {}

char Symbol::getCharacter() const {
    return character;
}

SymbolCategory Symbol::getCategory() const {
    auto code = static_cast<unsigned char>(character);
    if (code >= SYMBOL_CATEGORIES.size()) {
        return Invalid;
    }
    return SYMBOL_CATEGORIES[code];
}