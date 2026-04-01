#include "Source.hpp"

Source::Source() = default;

Source::Source(Cursor cursor) : _cursor(cursor) {}

const Cursor& Source::cursor() const {
    return _cursor;
}