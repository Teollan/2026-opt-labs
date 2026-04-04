#pragma once

#include <string>
#include <utility>
#include <vector>

#include <TableView.hpp>

using SymbolEntry = std::pair<std::string, size_t>;

class LiteralsTableView {
private:
    TableView<SymbolEntry> _table;

public:
    LiteralsTableView(
        std::string title,
        const std::vector<SymbolEntry>& data
    );

    void print() const;
};
