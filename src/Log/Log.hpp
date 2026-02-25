#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
class Log {
private:
    std::string name;
    std::vector<T> data;
    std::function<std::string(const T&)> formatter;
    std::ostream& out;

public:
    Log(std::string name, std::vector<T> data, std::ostream& out = std::cout);

    Log& setFormatter(std::function<std::string(const T&)> formatter);

    void print() const;
};

#include "Log.tpp"