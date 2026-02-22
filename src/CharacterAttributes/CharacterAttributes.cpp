#include "CharacterAttributes.hpp"

CharacterAttributes::CharacterAttributes() {
    for (size_t i = 0; i < ASCII_TABLE_SIZE; i++) {
        attributes[i] = Attribute::Invalid;
    }

    // Whitespace
    attributes[' '] = Attribute::Whitespace;
    attributes['\t'] = Attribute::Whitespace;
    attributes['\n'] = Attribute::Whitespace;
    attributes['\v'] = Attribute::Whitespace;
    attributes['\f'] = Attribute::Whitespace;
    attributes['\r'] = Attribute::Whitespace;

    // Digits
    for (char ch = '0'; ch <= '9'; ch++) {
        attributes[ch] = Attribute::Digit;
    }

    // Letters (A-Z)
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        attributes[ch] = Attribute::Letter;
    }

    // Delimiters
    attributes[';'] = Attribute::Delimiter;
    attributes['.'] = Attribute::Delimiter;
    attributes['='] = Attribute::Delimiter;
    attributes['\''] = Attribute::Delimiter;
    attributes[','] = Attribute::Delimiter;
    attributes[')'] = Attribute::Delimiter;
    attributes['$'] = Attribute::Delimiter;

    // Comment start
    attributes['('] = Attribute::Comment;
}

Attribute CharacterAttributes::lookup(char character) const {
    return attributes[static_cast<unsigned char>(character)];
}