#pragma once

#include <string>
#include <vector>

#include <SymbolStore.hpp>
#include <TableView.hpp>
#include <Tokenizer.hpp>

class TokensTableView {
private:
    TableView<Token> _table;

public:
    TokensTableView(
        std::string title,
        const SymbolStore& symbols,
        const std::vector<Token>& data
    );

    void print() const;
};
