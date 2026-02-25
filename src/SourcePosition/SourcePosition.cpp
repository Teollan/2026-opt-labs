#include "SourcePosition.hpp"

SourcePosition::SourcePosition() : SourcePosition(4) {}

SourcePosition::SourcePosition(uint8_t tabSize) : tabSize(tabSize) {};

void SourcePosition::advance(char character) {
    switch (character) {
        case '\n':
            _row++;
            _column = 0;
            break;

        case '\t':
            _column += tabSize;
            break;

        default:
            _column++;
            break;
    }
}

unsigned int SourcePosition::row() const {
    return _row;
}

unsigned int SourcePosition::column() const {
    return _column;
}