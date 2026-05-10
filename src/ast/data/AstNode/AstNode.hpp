#pragma once

#include <AstVisitor.hpp>
#include <Declaration.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(AstVisitor& visitor) const = 0;
};

class ConstantNode : public AstNode {
private:
    std::string _identifier;
    Value _value;
    size_t _row;
    size_t _column;

public:
    ConstantNode(
        std::string identifier, Value value, size_t row, size_t column
    );

    [[nodiscard]] const std::string& identifier() const;
    [[nodiscard]] const Value& value() const;
    [[nodiscard]] size_t row() const;
    [[nodiscard]] size_t column() const;

    void accept(AstVisitor& visitor) const override;
};

class StatementsNode : public AstNode {
public:
    void accept(AstVisitor& visitor) const override;
};

class ProgramNode : public AstNode {
private:
    std::string _identifier;
    std::vector<std::unique_ptr<ConstantNode>> _constants;
    std::unique_ptr<StatementsNode> _statements;
    size_t _row;
    size_t _column;

public:
    ProgramNode(std::string identifier, size_t row, size_t column);

    [[nodiscard]] const std::string& identifier() const;
    [[nodiscard]] const std::vector<std::unique_ptr<ConstantNode>>& constants(
    ) const;
    [[nodiscard]] const StatementsNode* statements() const;
    [[nodiscard]] size_t row() const;
    [[nodiscard]] size_t column() const;

    void addConstant(std::unique_ptr<ConstantNode> constant);
    void setStatements(std::unique_ptr<StatementsNode> statements);

    void accept(AstVisitor& visitor) const override;
};

class RootNode : public AstNode {
private:
    std::unique_ptr<ProgramNode> _program;

public:
    [[nodiscard]] const ProgramNode* program() const;

    void setProgram(std::unique_ptr<ProgramNode> program);

    void accept(AstVisitor& visitor) const override;
};
