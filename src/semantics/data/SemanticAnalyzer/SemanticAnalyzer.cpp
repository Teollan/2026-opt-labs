#include "SemanticAnalyzer.hpp"

#include <AstNode.hpp>
#include <Declaration.hpp>
#include <SemanticError.hpp>
#include <complex>
#include <optional>
#include <variant>
#include <vector>

namespace {

Type typeOf(const Value& value) {
    return std::visit(
        [](const auto& alternative) {
            using T = std::decay_t<decltype(alternative)>;
            if constexpr (
                std::is_same_v<T, std::complex<float>> ||
                std::is_same_v<T, float>
            ) {
                return Type::Float;
            } else {
                return Type::Integer;
            }
        },
        value
    );
}

std::vector<TypeModifier> modifiersOf(const Value& value) {
    return std::visit(
        [](const auto& alternative) -> std::vector<TypeModifier> {
            using T = std::decay_t<decltype(alternative)>;
            if constexpr (
                std::is_same_v<T, std::complex<int>> ||
                std::is_same_v<T, std::complex<float>>
            ) {
                return {TypeModifier::Complex};
            } else {
                return {};
            }
        },
        value
    );
}

}  // namespace

SemanticAnalyzer::SemanticAnalyzer(
    const AbstractSyntaxTree& ast,
    Logger<SemanticError>& logger
) :
    _ast(ast), _logger(logger) {}

void SemanticAnalyzer::analyze() {
    _ast.accept(*this);
}

const DeclarationsTable& SemanticAnalyzer::declarations() const {
    return _declarations;
}

void SemanticAnalyzer::visitRootNode(const RootNode& node) {
    if (node.program) {
        node.program->accept(*this);
    }
}

void SemanticAnalyzer::visitProgramNode(const ProgramNode& node) {
    auto [existing, inserted] = _declarations.declare({
        .identifier = node.identifier,
        .kind = DeclarationKind::Program,
        .type = std::nullopt,
        .value = std::nullopt,
        .modifiers = {},
        .row = node.row,
        .column = node.column,
    });

    if (!inserted) {
        _logger.message({
            .message = SemanticError::DuplicateIdentifier(
                node.identifier, existing->row, existing->column
            ),
            .row = node.row,
            .column = node.column,
        });
    }

    for (const auto& constant : node.constants) {
        constant->accept(*this);
    }
}

void SemanticAnalyzer::visitStatementsNode(const StatementsNode& node) {}

void SemanticAnalyzer::visitConstantDeclarationNode(const ConstantNode& node) {
    auto [existing, inserted] = _declarations.declare({
        .identifier = node.identifier,
        .kind = DeclarationKind::Constant,
        .type = typeOf(node.value),
        .value = node.value,
        .modifiers = modifiersOf(node.value),
        .row = node.row,
        .column = node.column,
    });

    if (!inserted) {
        _logger.message({
            .message = SemanticError::DuplicateIdentifier(
                node.identifier, existing->row, existing->column
            ),
            .row = node.row,
            .column = node.column,
        });
    }
}
