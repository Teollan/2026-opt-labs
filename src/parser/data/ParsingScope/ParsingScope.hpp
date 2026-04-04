#pragma once

#include <Stack.hpp>
#include <SyntaxData.hpp>
#include <Tree.hpp>

class ParsingScope {
    Stack<TreeNode<SyntaxData>*>* _stack;

public:
    explicit ParsingScope(Stack<TreeNode<SyntaxData>*>& stack);
    ~ParsingScope();

    ParsingScope(ParsingScope&& other) noexcept;
    ParsingScope(const ParsingScope&) = delete;
    ParsingScope& operator=(const ParsingScope&) = delete;
    ParsingScope& operator=(ParsingScope&&) = delete;
};
