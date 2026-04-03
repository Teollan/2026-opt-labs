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
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;
    TreeNode(TreeNode&&) = default;
    TreeNode& operator=(TreeNode&&) = default;

    const T& data() const;
    T& data();

    const std::vector<std::unique_ptr<TreeNode<T>>>& children() const;
    TreeNode<T>& grow(T data);
    void prune();
};

#include "TreeNode.tpp"
