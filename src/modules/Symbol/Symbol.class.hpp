#ifndef SYMBOL_CLASS_HPP
#define SYMBOL_CLASS_HPP

#include <cstddef>

enum SymbolCategory {
    Whitespace,
    Digit,
    Letter,
    SingleDelimiter,
    Comment,
    Invalid,
};

class Symbol {
private:
    char character;

public:
    Symbol() = default;
    Symbol(char character);

    char getCharacter() const;
    SymbolCategory getCategory() const;
};

#endif
