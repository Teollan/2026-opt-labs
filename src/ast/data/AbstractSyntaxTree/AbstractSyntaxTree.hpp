#pragma once

#include <AstNode.hpp>
#include <Declaration.hpp>
#include <SymbolStore.hpp>
#include <SyntaxData.hpp>
#include <Tree.hpp>
#include <TreeNode.hpp>
#include <memory>

class AbstractSyntaxTree {
private:
    const SymbolStore& _symbols;

    void foldAxiom(const TreeNode<SyntaxData>& node);
    std::unique_ptr<ProgramNode> foldProgram(const TreeNode<SyntaxData>& node);
    void foldBlock(const TreeNode<SyntaxData>& node, ProgramNode& program);
    void foldDeclarations(
        const TreeNode<SyntaxData>& node,
        ProgramNode& program
    );
    void foldConstantDeclarationsList(
        const TreeNode<SyntaxData>& node,
        ProgramNode& program
    );
    std::unique_ptr<ConstantNode> foldConstantDeclaration(
        const TreeNode<SyntaxData>& node
    );
    Value evaluateConstant(const TreeNode<SyntaxData>& node);
    Value evaluateComplexNumber(const TreeNode<SyntaxData>& node);

public:
    std::unique_ptr<RootNode> root;

    AbstractSyntaxTree(
        const Tree<SyntaxData>& tree, const SymbolStore& symbols
    );
    void accept(class AstVisitor& visitor) const;
};
