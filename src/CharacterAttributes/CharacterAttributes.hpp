#pragma once

#include <array>

#include "Constants.hpp"

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
    std::array<Attribute, ASCII_TABLE_SIZE> attributes;

public:
    CharacterAttributes();

    [[nodiscard]] Attribute lookup(char character) const;
};
