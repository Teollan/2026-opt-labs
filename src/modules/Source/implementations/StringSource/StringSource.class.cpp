#include "StringSource.class.hpp"

StringSource::StringSource(std::string source) :
    source(std::move(source)),
    iterator(this->source.begin()) {}

Symbol StringSource::getCurrentSymbol() {
    if (isEnd()) {
        return {'\0'};
    }

    return {*iterator};
}

Symbol StringSource::advance() {
    if (!isEnd()) {
        ++iterator;
    }

    return getCurrentSymbol();
}

bool StringSource::isEnd() {
    return iterator == source.end();
}
