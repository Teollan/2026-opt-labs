#pragma once

#include <Cursor.hpp>

class Source {
protected:
    Cursor _cursor;

public:
    Source();
    Source(Cursor cursor);

    virtual ~Source() = default;

    virtual char current() = 0;
    virtual char read() = 0;
    virtual bool done() = 0;

    [[nodiscard]] const Cursor& cursor() const;
};
