#include "AstNode.hpp"

void ConstantNode::accept(class AstVisitor& visitor) {
    visitor.visitConstantDeclarationNode(*this);
}

void StatementsNode::accept(class AstVisitor& visitor) {
    visitor.visitStatementsNode(*this);
}

void ProgramNode::accept(class AstVisitor& visitor) {
    visitor.visitProgramNode(*this);
}

void RootNode::accept(class AstVisitor& visitor) {
    visitor.visitRootNode(*this);
}