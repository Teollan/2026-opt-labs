#include "Source.hpp"

Source::Source() = default;

Source::Source(SourcePosition position) : position(position) {}

unsigned int Source::row() const {
    return position.row();
}

unsigned int Source::column() const {
    return position.column();
}