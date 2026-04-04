#include "FileSource.hpp"

FileSource::FileSource(const std::string& filePath) : 
    file(filePath),
    character(static_cast<char>(file.get())) {}

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