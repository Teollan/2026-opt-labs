#ifndef STRING_SOURCE_CLASS_HPP
#define STRING_SOURCE_CLASS_HPP

#include <string>
#include "Source.class.hpp"

class StringSource : public Source {
private:
    std::string source;
    std::string::iterator iterator;

public:
    StringSource(const std::string source);

    char current() override;
    char read() override;
    bool done() override;
};

#endif