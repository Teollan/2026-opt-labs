#include "TokensTableView.hpp"

#include <format>

std::string getTypeLabel(SymbolType type) {
    switch (type) {
        case SymbolType::Ascii:
            return "Delimiter";
        case SymbolType::MultiDelimiter:
            return "Multi-Delimiter";
        case SymbolType::Keyword:
            return "Keyword";
        case SymbolType::Literal:
            return "Literal";
        case SymbolType::Identifier:
            return "Identifier";
        default:
            return "N/A";
    }
}

TokensTableView::TokensTableView(
    std::string title,
    const SymbolStore& symbols,
    const std::vector<Token>& data
) :
    _table(std::move(title), data) {
    _table
        .addColumn(
            "Code", "{:>5}",
            [](const auto& t) { return std::to_string(t.code); }
        )
        .addColumn(
            "Row", "{:>3}",
            [](const auto& t) { return std::to_string(t.row + 1); }
        )
        .addColumn(
            "Col", "{:>3}",
            [](const auto& t) { return std::to_string(t.column + 1); }
        )
        .addColumn(
            "Type", "{:<15}",
            [&symbols](const auto& t) {
                return getTypeLabel(symbols.lookupType(t.code));
            }
        )
        .addColumn(
            "Value", "{:<30}",
            [&symbols](const auto& t) {
                return std::string(symbols.lookup(t.code));
            }
        );
}

void TokensTableView::print() const {
    _table.print();
}
