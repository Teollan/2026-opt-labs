#pragma once

#include <string>

#include <Parser.hpp>
#include <SymbolStore.hpp>
#include <TreeView.hpp>

class SyntaxTreeView {
private:
    TreeView<SyntaxData> _tree;

public:
    SyntaxTreeView(
        std::string title,
        const Tree<SyntaxData>& tree,
        const SymbolStore& symbols
    );

    void print() const;
};
