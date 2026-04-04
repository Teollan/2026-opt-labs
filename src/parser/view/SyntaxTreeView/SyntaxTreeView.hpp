#pragma once

#include <string>

#include <Parser.hpp>
#include <TreeView.hpp>

class SyntaxTreeView {
private:
    TreeView<SyntaxData> _tree;

public:
    SyntaxTreeView(std::string title, const Tree<SyntaxData>& tree);

    void print() const;
};
