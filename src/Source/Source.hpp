#pragma once

#include <SourcePosition.hpp>

class Source {
protected:
    SourcePosition position;

public:
    Source();
    Source(SourcePosition position);

    virtual ~Source() = default;

    virtual char current() = 0;
    virtual char read() = 0;
    virtual bool done() = 0;

    [[nodiscard]] unsigned int row() const;
    [[nodiscard]] unsigned int column() const;
};

