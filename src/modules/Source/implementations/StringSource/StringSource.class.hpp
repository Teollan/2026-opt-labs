#ifndef STRING_SOURCE_CLASS_HPP
#define STRING_SOURCE_CLASS_HPP

#include <string>
#include <Symbol.class.hpp>
#include <Source.interface.hpp>

class StringSource : public Source {
private:
    std::string source;
    std::string::iterator iterator;

public:
    StringSource(const std::string source);

    Symbol getCurrentSymbol() override;
    Symbol advance() override;
    bool isEnd() override;
};

#endif