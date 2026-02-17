#include "Source.class.hpp"

void Source::advance(char character) {
    if (character == '\n') {
        _row++;
        _column = 0;
    } else {
        _column++;
    }
}

unsigned int Source::row() const {
    return _row;
}

unsigned int Source::column() const {
    return _column;
}