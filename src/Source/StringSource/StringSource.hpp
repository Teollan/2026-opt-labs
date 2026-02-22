#pragma once

#include <string>
#include "Source.hpp"

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

