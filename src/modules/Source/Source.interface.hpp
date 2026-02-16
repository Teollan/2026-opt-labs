#ifndef SOURCE_INTERFACE_HPP
#define SOURCE_INTERFACE_HPP

#include <Symbol.class.hpp>

class Source {
public:
    virtual ~Source() = default;

    virtual Symbol getCurrentSymbol() = 0;
    virtual Symbol advance() = 0;
    virtual bool isEnd() = 0;
};

#endif