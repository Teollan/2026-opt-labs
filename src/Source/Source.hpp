#pragma once

class Source {
private:
    unsigned int _row = 0;
    unsigned int _column = 0;

protected:
    void advance(char character);

public:
    virtual ~Source() = default;

    virtual char current() = 0;
    virtual char read() = 0;
    virtual bool done() = 0;

    unsigned int row() const;
    unsigned int column() const;
};

