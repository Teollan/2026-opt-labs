#include "AbstractSyntaxTree.hpp"

#include <AstNode.hpp>
#include <AstVisitor.hpp>
#include <Rules.hpp>
#include <complex>
#include <memory>
#include <optional>
#include <string>

AbstractSyntaxTree::AbstractSyntaxTree(
    const Tree<SyntaxData>& tree,
    const SymbolStore& symbols
) :
    _symbols(symbols), root(std::make_unique<RootNode>()) {
    foldAxiom(tree.root());
}

void AbstractSyntaxTree::accept(AstVisitor& visitor) const {
    root->accept(visitor);
}

void AbstractSyntaxTree::foldAxiom(const TreeNode<SyntaxData>& node) {
    const auto& signalProgram = *node.children()[0];
    const auto& program = *signalProgram.children()[0];

    root->setProgram(foldProgram(program));
}

std::unique_ptr<ProgramNode> AbstractSyntaxTree::foldProgram(
    const TreeNode<SyntaxData>& node
) {
    const auto& procedureIdentifier = *node.children()[0];
    const auto& identifierTerminal = *procedureIdentifier.children()[0];
    const auto& identifierToken = *identifierTerminal.data().token;

    auto program = std::make_unique<ProgramNode>(
        _symbols.lookup(identifierToken.code), identifierToken.row,
        identifierToken.column
    );

    const auto& block = *node.children()[1];
    foldBlock(block, *program);

    return program;
}

void AbstractSyntaxTree::foldBlock(
    const TreeNode<SyntaxData>& node,
    ProgramNode& program
) {
    const auto& declarations = *node.children()[0];

    foldDeclarations(declarations, program);
    program.setStatements(std::make_unique<StatementsNode>());
}

void AbstractSyntaxTree::foldDeclarations(
    const TreeNode<SyntaxData>& node,
    ProgramNode& program
) {
    const auto& constantDeclarations = *node.children()[0];

    if (constantDeclarations.data().rule ==
        RuleKey::ConstantDeclarationsEmpty) {
        return;
    }

    const auto& list = *constantDeclarations.children()[0];
    foldConstantDeclarationsList(list, program);
}

void AbstractSyntaxTree::foldConstantDeclarationsList(
    const TreeNode<SyntaxData>& node,
    ProgramNode& program
) {
    if (node.data().rule == RuleKey::ConstantDeclarationsListEmpty) {
        return;
    }

    const auto& declaration = *node.children()[0];
    program.addConstant(foldConstantDeclaration(declaration));

    const auto& rest = *node.children()[1];
    foldConstantDeclarationsList(rest, program);
}

std::unique_ptr<ConstantNode> AbstractSyntaxTree::foldConstantDeclaration(
    const TreeNode<SyntaxData>& node
) {
    const auto& constantIdentifier = *node.children()[0];
    const auto& identifierTerminal = *constantIdentifier.children()[0];
    const auto& identifierToken = *identifierTerminal.data().token;

    const auto& constantValue = *node.children()[1];

    return std::make_unique<ConstantNode>(
        _symbols.lookup(identifierToken.code), evaluateConstant(constantValue),
        identifierToken.row, identifierToken.column
    );
}

Value AbstractSyntaxTree::evaluateConstant(const TreeNode<SyntaxData>& node) {
    const auto& complexNumber = *node.children()[0];

    return evaluateComplexNumber(complexNumber);
}

Value AbstractSyntaxTree::evaluateComplexNumber(
    const TreeNode<SyntaxData>& node
) {
    const auto& leftPart = *node.children()[0];
    const auto& rightPart = *node.children()[1];

    std::optional<std::string> leftPartStr;
    if (!leftPart.children().empty()) {
        leftPartStr =
            _symbols.lookup(leftPart.children()[0]->data().token->code);
    }

    std::optional<std::string> rightPartStr;
    if (!rightPart.children().empty()) {
        rightPartStr =
            _symbols.lookup(rightPart.children()[0]->data().token->code);
    }

    if (rightPart.data().rule == RuleKey::RightPartExp) {
        auto magnitude = std::stof(leftPartStr.value_or("1"));
        auto exponent = std::stof(rightPartStr.value_or("0"));

        return Value{std::polar(magnitude, exponent)};
    }

    auto real = std::stoi(leftPartStr.value_or("0"));
    auto imaginary = std::stoi(rightPartStr.value_or("0"));

    return Value{std::complex<int>{real, imaginary}};
}
