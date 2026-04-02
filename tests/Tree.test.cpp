#include <gtest/gtest.h>

#include <Tree.hpp>

class TreeTest : public ::testing::Test {};

// Tree constructor

TEST_F(TreeTest, ConstructorSetsRootData) {
    Tree<int> tree(42);
    EXPECT_EQ(tree.root().data(), 42);
}

TEST_F(TreeTest, RootReturnsReference) {
    Tree<std::string> tree("root");
    auto& root = tree.root();
    EXPECT_EQ(root.data(), "root");
}

// TreeNode::grow

TEST_F(TreeTest, GrowReturnsChildReference) {
    Tree<int> tree(1);
    auto& child = tree.root().grow(2);
    EXPECT_EQ(child.data(), 2);
}

TEST_F(TreeTest, GrowMultipleChildren) {
    Tree<int> tree(1);
    auto& child1 = tree.root().grow(2);
    auto& child2 = tree.root().grow(3);
    EXPECT_EQ(child1.data(), 2);
    EXPECT_EQ(child2.data(), 3);
}

TEST_F(TreeTest, GrowNestedChildren) {
    Tree<int> tree(1);
    auto& child = tree.root().grow(2);
    auto& grandchild = child.grow(3);
    EXPECT_EQ(grandchild.data(), 3);
}

TEST_F(TreeTest, GrowDeepTree) {
    Tree<int> tree(0);
    auto* node = &tree.root();

    for (int i = 1; i <= 10; i++) {
        node = &node->grow(i);
    }

    EXPECT_EQ(node->data(), 10);
}

// Works with strings

TEST_F(TreeTest, WorksWithStrings) {
    Tree<std::string> tree("root");
    auto& child = tree.root().grow("child");
    auto& grandchild = child.grow("grandchild");
    EXPECT_EQ(grandchild.data(), "grandchild");
}
