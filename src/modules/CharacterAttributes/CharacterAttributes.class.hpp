#ifndef CHARACTER_ATTRIBUTES_CLASS_HPP
#define CHARACTER_ATTRIBUTES_CLASS_HPP

#include <array>

enum class Attribute {
    Whitespace,
    Digit,
    Letter,
    Delimiter,
    Comment,
    Invalid,
};

class CharacterAttributes {
private:
    std::array<Attribute, 256> attributes;

public:
    CharacterAttributes();

    Attribute lookup(char character) const;
};

#endif