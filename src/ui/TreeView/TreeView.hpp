#pragma once

#include <functional>
#include <iostream>
#include <string>

#include <ui.constants.hpp>
#include <Tree.hpp>

template <typename T>
class TreeView {
private:
    std::string _title;
    const Tree<T>& _tree;
    std::ostream& _out;
    std::function<std::string(const T&)> _nodeFormatter;
    std::function<std::string(const T&)> _edgeFormatter;

    void printNode(
        const TreeNode<T>& node,
        const std::string& prefix,
        bool isLast
    ) const;

public:
    TreeView(std::string title, const Tree<T>& tree, std::ostream& out = std::cout);

    TreeView& setNodeFormatter(std::function<std::string(const T&)> formatter);
    TreeView& setEdgeFormatter(std::function<std::string(const T&)> formatter);

    void print() const;
};

#include "TreeView.tpp"
