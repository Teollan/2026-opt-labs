#pragma once

#include <TreeNode.hpp>

template <typename T>
class Tree {
private:
    std::unique_ptr<TreeNode<T>> _root;

public:
    Tree(T data);

    TreeNode<T>& root();
};

#include "Tree.tpp"
