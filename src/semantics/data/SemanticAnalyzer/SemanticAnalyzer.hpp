#pragma once

#include <DeclarationsTable.hpp>
#include <Logger.hpp>
#include <SemanticError.hpp>
#include <SymbolStore.hpp>
#include <SyntaxData.hpp>
#include <Tree.hpp>
#include <utility>

#include "Declaration.hpp"

class SemanticAnalyzer {
private:
    const SymbolStore& _symbols;
    const Tree<SyntaxData>& _tree;
    Logger<SemanticError>& _logger;

    DeclarationsTable _declarations;

    void analyzeProgramDeclaration(const TreeNode<SyntaxData>& node);
    void analyzeConstantDeclaration(const TreeNode<SyntaxData>& node);

    std::pair<Type, Value> evaluateComplexNumber(
        const TreeNode<SyntaxData>& node
    );

public:
    SemanticAnalyzer(
        const SymbolStore& symbols,
        const Tree<SyntaxData>& tree,
        Logger<SemanticError>& logger
    );

    void analyze();
    const DeclarationsTable& declarations() const;
};
