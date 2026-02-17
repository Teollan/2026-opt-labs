#ifndef FILE_SOURCE_CLASS_HPP
#define FILE_SOURCE_CLASS_HPP

#include <string>
#include <fstream>
#include "Source.class.hpp"

class FileSource : public Source {
private:
    std::ifstream file;
    char character;

public:
    FileSource(const std::string& filePath);

    char current() override;
    char read() override;
    bool done() override;
};

#endif