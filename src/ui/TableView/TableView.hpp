#pragma once

#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <ui.constants.hpp>

template <typename T>
class TableView {
private:
    struct Column {
        std::string name;
        std::string format;
        std::function<std::string(const T&)> extractor;
    };

    std::string _title;
    std::vector<T> _data;
    std::vector<Column> _columns;
    std::ostream& _out;

    [[nodiscard]] std::string border(
        const std::string& left,
        const std::string& junction,
        const std::string& right
    ) const;
    [[nodiscard]] std::string header() const;
    [[nodiscard]] std::string row(const T& item) const;
    [[nodiscard]] size_t columnWidth(const Column& col) const;
    static size_t parseWidth(const std::string& fmt);

public:
    TableView(
        std::string title,
        std::vector<T> data,
        std::ostream& out = std::cout
    );

    TableView& addColumn(
        const std::string& colName,
        const std::string& format,
        std::function<std::string(const T&)> extractor
    );

    void print() const;
};

#include "TableView.tpp"
