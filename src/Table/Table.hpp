#pragma once

#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
class Table {
private:
    struct Column {
        std::string name;
        std::string format;
        std::function<std::string(const T&)> extractor;
    };

    std::string name;
    std::vector<T> data;
    std::vector<Column> columns;
    std::ostream& out;

    [[nodiscard]] std::string border() const;
    [[nodiscard]] std::string header() const;
    [[nodiscard]] std::string row(const T& item) const;
    [[nodiscard]] size_t columnWidth(const Column& col) const;
    static size_t parseWidth(const std::string& fmt);

public:
    Table(std::string name, std::vector<T> data, std::ostream& out = std::cout);

    Table& addColumn(
        const std::string& colName,
        const std::string& format,
        std::function<std::string(const T&)> extractor
    );

    void print() const;
};

#include "Table.tpp"
