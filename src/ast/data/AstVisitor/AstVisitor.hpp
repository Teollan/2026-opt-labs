#pragma once

class ConstantNode;
class StatementsNode;
class ProgramNode;
class RootNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    virtual void visitConstantNode(const ConstantNode& node) = 0;
    virtual void visitStatementsNode(const StatementsNode& node) = 0;
    virtual void visitProgramNode(const ProgramNode& node) = 0;
    virtual void visitRootNode(const RootNode& node) = 0;
};