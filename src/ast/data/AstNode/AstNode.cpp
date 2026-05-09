#include "AstNode.hpp"

void ConstantNode::accept(AstVisitor& visitor) const {
    visitor.visitConstantDeclarationNode(*this);
}

void StatementsNode::accept(AstVisitor& visitor) const {
    visitor.visitStatementsNode(*this);
}

void ProgramNode::accept(AstVisitor& visitor) const {
    visitor.visitProgramNode(*this);
}

void RootNode::accept(AstVisitor& visitor) const {
    visitor.visitRootNode(*this);
}