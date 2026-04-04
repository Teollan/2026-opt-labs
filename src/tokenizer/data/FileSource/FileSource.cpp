#include "FileSource.hpp"

#include <stdexcept>

FileSource::FileSource(const std::string& filePath) :
    file(filePath),
    character(static_cast<char>(file.get())) {
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
}

char FileSource::current() {
    return character;
}

char FileSource::read() {
    _cursor.advance(character);
    character = static_cast<char>(file.get());

    return character;
}

bool FileSource::done() {
    return file.eof();
}