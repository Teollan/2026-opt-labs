#pragma once

#include <cstdint>

class Cursor {
private:
    uint8_t tabSize = 4;

    unsigned int _row = 0;
    unsigned int _column = 0;

public:
    Cursor();
    Cursor(uint8_t tabSize);

    void advance(char character);
    [[nodiscard]] unsigned int row() const;
    [[nodiscard]] unsigned int column() const;
};
