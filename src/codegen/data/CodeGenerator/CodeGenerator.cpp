#include "CodeGenerator.hpp"

#include <complex>
#include <format>
#include <type_traits>
#include <variant>

CodeGenerator::CodeGenerator(
    const AbstractSyntaxTree& ast,
    const AssemblyFormatter& formatter
) :
    _ast(ast), _formatter(formatter) {}

void CodeGenerator::generate() {
    _out.str("");
    _out.clear();

    _ast.accept(*this);
}

void CodeGenerator::visitRootNode(const RootNode& node) {
    if (node.program) {
        node.program->accept(*this);
    }
}

void CodeGenerator::visitProgramNode(const ProgramNode& node) {
    if (!node.constants.empty()) {
        _out << _formatter.sectionLine(".data");

        for (const auto& constant : node.constants) {
            constant->accept(*this);
        }

        _out << _formatter.blankLine();
    }

    _out << _formatter.sectionLine(".text");
    _out << _formatter.instructionLine("global", "_start");

    if (node.statements) {
        node.statements->accept(*this);
    }
}

void CodeGenerator::visitConstantDeclarationNode(const ConstantNode& node) {
    std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::complex<int>>) {
                _out << _formatter.constantLine(
                    node.identifier, "dd",
                    std::format("{}, {}", value.real(), value.imag())
                );
            } else if constexpr (std::is_same_v<T, std::complex<float>>) {
                _out << _formatter.constantLine(
                    node.identifier, "dd",
                    std::format("{:g}, {:g}", value.real(), value.imag())
                );
            } else if constexpr (std::is_same_v<T, int>) {
                _out << _formatter.constantLine(
                    node.identifier, "dd", std::format("{}", value)
                );
            } else if constexpr (std::is_same_v<T, float>) {
                _out << _formatter.constantLine(
                    node.identifier, "dd", std::format("{:g}", value)
                );
            }
        },
        node.value
    );
}

void CodeGenerator::visitStatementsNode(const StatementsNode&) {
    _out << _formatter.labelLine("_start");
    _out << _formatter.instructionLine("nop");
    _out << _formatter.instructionLine("mov", "eax, 60");
    _out << _formatter.instructionLine("xor", "edi, edi");
    _out << _formatter.instructionLine("syscall");
}

std::string CodeGenerator::output() const {
    return _out.str();
}
