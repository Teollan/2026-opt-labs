#pragma once

#include <vector>
#include <memory>

template <typename T>
class TreeNode {
private:
    T _data;
    std::vector<std::unique_ptr<TreeNode<T>>> _children;

public:
    TreeNode(T data);

    const T& data() const;
    TreeNode<T>& grow(T data);
};

#include "TreeNode.tpp"
