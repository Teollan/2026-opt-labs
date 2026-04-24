#pragma once

#include <string>

#include <Parser.hpp>
#include <SymbolStore.hpp>

class DotTreeView {
private:
    const Tree<SyntaxData>& _tree;
    const SymbolStore& _symbols;

    std::string nodeLabel(const SyntaxData& data) const;
    std::string edgeLabel(const SyntaxData& data) const;

    void writeNode(
        std::ostream& out,
        const TreeNode<SyntaxData>& node,
        int& id
    ) const;

public:
    DotTreeView(const Tree<SyntaxData>& tree, const SymbolStore& symbols);

    void write(const std::string& filename) const;
};
