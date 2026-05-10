#include "AstNode.hpp"

#include <utility>

// --- ConstantNode ---

ConstantNode::ConstantNode(
    std::string identifier,
    Value value,
    size_t row,
    size_t column
) :
    _identifier(std::move(identifier)),
    _value(std::move(value)),
    _row(row),
    _column(column) {}

const std::string& ConstantNode::identifier() const {
    return _identifier;
}

const Value& ConstantNode::value() const {
    return _value;
}

size_t ConstantNode::row() const {
    return _row;
}

size_t ConstantNode::column() const {
    return _column;
}

void ConstantNode::accept(AstVisitor& visitor) const {
    visitor.visitConstantNode(*this);
}

// --- StatementsNode ---

void StatementsNode::accept(AstVisitor& visitor) const {
    visitor.visitStatementsNode(*this);
}

// --- ProgramNode ---

ProgramNode::ProgramNode(std::string identifier, size_t row, size_t column) :
    _identifier(std::move(identifier)), _row(row), _column(column) {}

const std::string& ProgramNode::identifier() const {
    return _identifier;
}

const std::vector<std::unique_ptr<ConstantNode>>& ProgramNode::constants(
) const {
    return _constants;
}

const StatementsNode* ProgramNode::statements() const {
    return _statements.get();
}

size_t ProgramNode::row() const {
    return _row;
}

size_t ProgramNode::column() const {
    return _column;
}

void ProgramNode::addConstant(std::unique_ptr<ConstantNode> constant) {
    _constants.push_back(std::move(constant));
}

void ProgramNode::setStatements(std::unique_ptr<StatementsNode> statements) {
    _statements = std::move(statements);
}

void ProgramNode::accept(AstVisitor& visitor) const {
    visitor.visitProgramNode(*this);
}

// --- RootNode ---

const ProgramNode* RootNode::program() const {
    return _program.get();
}

void RootNode::setProgram(std::unique_ptr<ProgramNode> program) {
    _program = std::move(program);
}

void RootNode::accept(AstVisitor& visitor) const {
    visitor.visitRootNode(*this);
}
