#pragma once

class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    virtual void visitConstantDeclarationNode(
        const class ConstantNode& node
    ) = 0;

    virtual void visitProgramNode(const class ProgramNode& node) = 0;

    virtual void visitRootNode(const class RootNode& node) = 0;
};