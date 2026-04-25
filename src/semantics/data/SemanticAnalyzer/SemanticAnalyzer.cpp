#include "SemanticAnalyzer.hpp"

#include <Declaration.hpp>
#include <Rules.hpp>
#include <SemanticError.hpp>
#include <complex>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

SemanticAnalyzer::SemanticAnalyzer(
    const SymbolStore& symbols,
    const Tree<SyntaxData>& tree,
    Logger<SemanticError>& logger
) :
    _symbols(symbols), _tree(tree), _logger(logger) {}

void SemanticAnalyzer::analyze() {
    analyzeNode(_tree.root());
}

void SemanticAnalyzer::analyzeNode(const TreeNode<SyntaxData>& node) {
    const auto& data = node.data();

    if (data.rule == RuleKey::Program) {
        analyzeProgramDeclaration(node);
    } else if (data.rule == RuleKey::ConstantDeclaration) {
        analyzeConstantDeclaration(node);
    }

    for (const auto& child : node.children()) {
        analyzeNode(*child);
    }
}

void SemanticAnalyzer::analyzeProgramDeclaration(
    const TreeNode<SyntaxData>& node
) {
    const auto& identifierData = node.children()[0]->children()[0]->data();
    const auto identifier = _symbols.lookup(identifierData.token->code);

    try {
        _declarations.declare({
            .identifier = identifier,
            .kind = DeclarationKind::Program,
            .type = std::nullopt,
            .value = std::nullopt,
            .modifiers = {},
        });
    } catch (const std::runtime_error&) {
        SemanticError error(
            SemanticError::DuplicateIdentifier(identifier),
            identifierData.token->row, identifierData.token->column
        );

        _logger.message(error);
    }
}

void SemanticAnalyzer::analyzeConstantDeclaration(
    const TreeNode<SyntaxData>& node
) {
    const auto& identifierNode = node.children()[0]->children()[0];
    const auto& identifier =
        _symbols.lookup(identifierNode->data().token->code);
    const auto& valueNode = node.children()[1];

    const auto [type, value] = evaluateComplexNumber(*valueNode);

    try {
        _declarations.declare({
            .identifier = identifier,
            .kind = DeclarationKind::Constant,
            .type = type,
            .value = value,
            .modifiers = {TypeModifier::Complex},
        });
    } catch (const std::runtime_error&) {
        SemanticError error(
            SemanticError::DuplicateIdentifier(identifier),
            identifierNode->data().token->row,
            identifierNode->data().token->column
        );

        _logger.message(error);
    }
}

const DeclarationsTable& SemanticAnalyzer::declarations() const {
    return _declarations;
}

std::pair<Type, Value> SemanticAnalyzer::evaluateComplexNumber(
    const TreeNode<SyntaxData>& node
) {
    const auto& leftPartNode = node.children()[0]->children()[0];
    std::optional<std::string> leftPartStr = std::nullopt;
    if (!leftPartNode->children().empty()) {
        leftPartStr =
            _symbols.lookup(leftPartNode->children()[0]->data().token->code);
    }

    const auto& rightPartNode = node.children()[0]->children()[1];
    std::optional<std::string> rightPartStr = std::nullopt;
    if (!rightPartNode->children().empty()) {
        rightPartStr =
            _symbols.lookup(rightPartNode->children()[0]->data().token->code);
    }

    if (rightPartNode->data().rule == RuleKey::RightPartExp) {
        auto magnitude = std::stof(leftPartStr.value_or("1"));
        auto exponent = std::stof(rightPartStr.value_or("0"));
        auto value = std::polar(magnitude, exponent);

        return {Type::Float, Value{value}};
    }

    auto real = std::stoi(leftPartStr.value_or("0"));
    auto imaginary = std::stoi(rightPartStr.value_or("0"));
    auto value = std::complex<int>{real, imaginary};

    return {Type::Integer, Value{value}};
}
