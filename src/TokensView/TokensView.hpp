#pragma once

#include <iostream>
#include <vector>
#include "Tokenizer.hpp"
#include "SymbolStore.hpp"

class TokensView {
private:
    std::ostream& out;

    static std::string typeLabel(SymbolType type);

public:
    TokensView(std::ostream& out);

    void print(const std::vector<Token>& tokens, const SymbolStore& symbols) const;
};
