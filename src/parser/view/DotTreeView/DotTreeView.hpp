#pragma once

#include <string>

#include <Parser.hpp>

class DotTreeView {
private:
    const Tree<SyntaxData>& _tree;

    void writeNode(
        std::ostream& out,
        const TreeNode<SyntaxData>& node,
        int& id
    ) const;

public:
    DotTreeView(const Tree<SyntaxData>& tree);

    void write(const std::string& filename) const;
};
