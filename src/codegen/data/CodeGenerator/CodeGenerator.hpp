#pragma once

#include <AbstractSyntaxTree.hpp>
#include <AssemblyFormatter.hpp>
#include <AstNode.hpp>
#include <AstVisitor.hpp>
#include <sstream>
#include <string>

class CodeGenerator : public AstVisitor {
private:
    const AbstractSyntaxTree& _ast;
    const AssemblyFormatter& _formatter;
    std::ostringstream _out;

    void visitRootNode(const RootNode& node) override;
    void visitProgramNode(const ProgramNode& node) override;
    void visitConstantNode(const ConstantNode& node) override;
    void visitStatementsNode(const StatementsNode& node) override;

public:
    explicit CodeGenerator(
        const AbstractSyntaxTree& ast,
        const AssemblyFormatter& formatter
    );

    void generate();
    [[nodiscard]] std::string output() const;
};
