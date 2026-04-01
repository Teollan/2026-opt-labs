#include "Source.hpp"

Source::Source() = default;

Source::Source(Cursor cursor) : cursor(cursor) {}

unsigned int Source::row() const {
    return cursor.row();
}

unsigned int Source::column() const {
    return cursor.column();
}