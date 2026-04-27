#include "TokensTableView.hpp"

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
            [](const auto& token) { return std::to_string(token.code); }
        )
        .addColumn(
            "Row", "{:>3}",
            [](const auto& token) { return std::to_string(token.row + 1); }
        )
        .addColumn(
            "Col", "{:>3}",
            [](const auto& token) { return std::to_string(token.column + 1); }
        )
        .addColumn(
            "Type", "{:<15}",
            [&symbols](const auto& token) {
                return getTypeLabel(symbols.lookupType(token.code));
            }
        )
        .addColumn("Value", "{:<30}", [&symbols](const auto& token) {
            return std::string(symbols.lookup(token.code));
        });
}

void TokensTableView::print() const {
    _table.print();
}
