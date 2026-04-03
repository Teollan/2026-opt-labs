#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
class Logger {
private:
    std::string _name;
    std::function<std::string(const T&)> _formatter;
    std::ostream& _out;
    std::vector<T> _messages;

public:
    Logger(
        std::string name,
        std::function<std::string(const T&)> formatter,
        std::ostream& out = std::cout
    );

    void message(const T& data);

    [[nodiscard]] const std::vector<T>& messages() const;
    void clear();
};

#include "Logger.tpp"
