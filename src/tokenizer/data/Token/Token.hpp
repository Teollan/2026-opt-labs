#pragma once

#include <cstddef>

struct Token {
    size_t code;
    size_t row;
    size_t column;
};
