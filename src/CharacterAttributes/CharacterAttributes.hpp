#pragma once

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

    [[nodiscard]] Attribute lookup(char character) const;
};

