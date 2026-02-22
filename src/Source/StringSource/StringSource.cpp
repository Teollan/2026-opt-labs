#include "StringSource.hpp"

StringSource::StringSource(std::string source) :
    source(std::move(source)),
    iterator(this->source.begin()) {}

char StringSource::current() {
    return *iterator;
}

char StringSource::read() {
    position.advance(*iterator);
    iterator++;

    return *iterator;
}

bool StringSource::done() {
    return iterator == source.end();
}
