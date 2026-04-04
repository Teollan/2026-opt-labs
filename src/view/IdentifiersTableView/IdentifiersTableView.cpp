#include "IdentifiersTableView.hpp"

IdentifiersTableView::IdentifiersTableView(
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

void IdentifiersTableView::print() const {
    _table.print();
}
