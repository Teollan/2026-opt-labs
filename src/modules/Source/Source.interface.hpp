#ifndef SOURCE_INTERFACE_HPP
#define SOURCE_INTERFACE_HPP

class Source {
public:
    virtual ~Source() = default;

    virtual char current() = 0;
    virtual char read() = 0;
    virtual bool done() = 0;
};

#endif