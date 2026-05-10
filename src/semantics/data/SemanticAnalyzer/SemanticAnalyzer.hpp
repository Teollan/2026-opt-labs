#pragma once

#include <AbstractSyntaxTree.hpp>
#include <AstVisitor.hpp>
#include <Declaration.hpp>
#include <DeclarationsTable.hpp>
#include <Logger.hpp>
#include <SemanticError.hpp>

class SemanticAnalyzer : public AstVisitor {
private:
    const AbstractSyntaxTree& _ast;
    Logger<SemanticError>& _logger;

    DeclarationsTable _declarations;

public:
    SemanticAnalyzer(
        const AbstractSyntaxTree& ast, Logger<SemanticError>& logger
    );

    void analyze();
    [[nodiscard]] const DeclarationsTable& declarations() const;

    void visitRootNode(const RootNode& node) override;
    void visitProgramNode(const ProgramNode& node) override;
    void visitConstantNode(const ConstantNode& node) override;
    void visitStatementsNode(const StatementsNode& node) override;
};
