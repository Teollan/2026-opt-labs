#pragma once

#include <AstVisitor.hpp>
#include <Declaration.hpp>
#include <TreeNode.hpp>
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
public:
    std::string identifier;
    Value value;
    size_t row = 0;
    size_t column = 0;

    void accept(AstVisitor& visitor) const override;
};

class StatementsNode : public AstNode {
public:
    void accept(AstVisitor& visitor) const override;
};

class ProgramNode : public AstNode {
public:
    std::string identifier;
    std::vector<std::unique_ptr<ConstantNode>> constants;
    std::unique_ptr<StatementsNode> statements;
    size_t row = 0;
    size_t column = 0;

    void accept(AstVisitor& visitor) const override;
};

class RootNode : public AstNode {
public:
    std::unique_ptr<ProgramNode> program;

    void accept(AstVisitor& visitor) const override;
};