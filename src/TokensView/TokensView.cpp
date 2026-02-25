#include "TokensView.hpp"

#include <format>

TokensView::TokensView(std::ostream& out) : out(out) {}

std::string TokensView::typeLabel(SymbolType type) {
    switch (type) {
        case SymbolType::Delimiter:
            return "Delimiter";
        case SymbolType::Keyword:
            return "Keyword";
        case SymbolType::Literal:
            return "Literal";
        case SymbolType::Identifier:
            return "Identifier";
    }
}

void TokensView::print(const std::vector<Token>& tokens, const SymbolStore& symbols) const {
    std::string border = "+-------+-----+-----+------------+---------+";

    std::string header =
        std::format("| {:>5} | {:>3} | {:>3} | {:<10} | {:<7} |", "Code", "Row", "Col", "Type", "Value");

    out << border << "\n";
    out << header << "\n";
    out << border << "\n";

    for (const auto& token : tokens) {
        auto value = symbols.lookup(token.code);
        auto type = typeLabel(symbols.lookupType(token.code));

        out << std::format(
                   "| {:>5} | {:>3} | {:>3} | {:<10} | {:<7} |", token.code, token.row + 1, token.column + 1, type,
                   value
               )
            << "\n";
    }

    out << border << "\n";
}
