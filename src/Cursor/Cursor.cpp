#include "Cursor.hpp"

Cursor::Cursor() : Cursor(4) {}

Cursor::Cursor(uint8_t tabSize) : tabSize(tabSize) {};

void Cursor::advance(char character) {
    switch (character) {
        case '\r':
            break;

        case '\n':
            _row++;
            _column = 0;
            break;

        case '\t':
            _column += tabSize - _column % tabSize;
            break;

        default:
            _column++;
            break;
    }
}

unsigned int Cursor::row() const {
    return _row;
}

unsigned int Cursor::column() const {
    return _column;
}
