#include "ParsingScope.hpp"

ParsingScope::ParsingScope(Stack<TreeNode<SyntaxData>*>& stack) :
    _stack(&stack) {}

ParsingScope::~ParsingScope() {
    if (_stack) {
        _stack->pop();
    }
}

ParsingScope::ParsingScope(ParsingScope&& other) noexcept :
    _stack(other._stack) {
    other._stack = nullptr;
}
