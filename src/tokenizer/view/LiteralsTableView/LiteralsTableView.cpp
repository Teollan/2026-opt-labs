#include "LiteralsTableView.hpp"

LiteralsTableView::LiteralsTableView(
    std::string title,
    const std::vector<SymbolEntry>& data
) :
    _table(std::move(title), data) {
    _table
        .addColumn(
            "Code", "{:>5}",
            [](const auto& pair) { return std::to_string(pair.second); }
        )
        .addColumn(
            "Value", "{:<30}",
            [](const auto& pair) { return pair.first; }
        );
}

void LiteralsTableView::print() const {
    _table.print();
}
