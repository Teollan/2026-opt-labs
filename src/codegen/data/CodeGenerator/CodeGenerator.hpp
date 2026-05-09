#pragma once

#include <AbstractSyntaxTree.hpp>
#include <AstNode.hpp>
#include <AstVisitor.hpp>
#include <sstream>
#include <string>

struct AssemblyLine {
    std::string label = "";
    std::string instruction = "";
    std::string operands = "";
    std::string comment = "";
};

class CodeGenerator : public AstVisitor {
private:
    const AbstractSyntaxTree& _ast;
    std::ostringstream _out;

    void emitTextSection();
    void emitAssemblyLine(const AssemblyLine& line);
    void emitEmptyLine();

    void visitRootNode(const RootNode& node) override;
    void visitProgramNode(const ProgramNode& node) override;
    void visitConstantDeclarationNode(const ConstantNode& node) override;

public:
    explicit CodeGenerator(const AbstractSyntaxTree& ast);

    void generate();
    [[nodiscard]] std::string output() const;
};
