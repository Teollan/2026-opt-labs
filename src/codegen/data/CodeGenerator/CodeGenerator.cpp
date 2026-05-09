#include "CodeGenerator.hpp"

#include <complex>
#include <format>
#include <type_traits>
#include <variant>

CodeGenerator::CodeGenerator(const AbstractSyntaxTree& ast) : _ast(ast) {}

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
        emitAssemblyLine({.instruction = "section", .operands = ".data"});
        emitEmptyLine();

        for (const auto& constant : node.constants) {
            constant->accept(*this);
        }

        emitEmptyLine();
    }

    emitTextSection();
}

void CodeGenerator::visitConstantDeclarationNode(const ConstantNode& node) {
    std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::complex<int>>) {
                emitAssemblyLine({
                    .label = node.identifier,
                    .instruction = "dd",
                    .operands = std::format("{}, {}", value.real(), value.imag()),
                });
            } else if constexpr (std::is_same_v<T, std::complex<float>>) {
                emitAssemblyLine({
                    .label = node.identifier,
                    .instruction = "dd",
                    .operands =
                        std::format("{:g}, {:g}", value.real(), value.imag()),
                });
            } else if constexpr (std::is_same_v<T, int>) {
                emitAssemblyLine({
                    .label = node.identifier,
                    .instruction = "dd",
                    .operands = std::format("{}", value),
                });
            } else if constexpr (std::is_same_v<T, float>) {
                emitAssemblyLine({
                    .label = node.identifier,
                    .instruction = "dd",
                    .operands = std::format("{:g}", value),
                });
            }
        },
        node.value
    );
}

void CodeGenerator::emitTextSection() {
    emitAssemblyLine({.instruction = "section", .operands = ".text"});
    emitAssemblyLine({.instruction = "global", .operands = "_start"});
    emitEmptyLine();
    emitAssemblyLine({.label = "_start"});
    emitAssemblyLine({.instruction = "nop"});
    emitAssemblyLine({.instruction = "mov", .operands = "eax, 60"});
    emitAssemblyLine({.instruction = "xor", .operands = "edi, edi"});
    emitAssemblyLine({.instruction = "syscall"});
}

void CodeGenerator::emitAssemblyLine(const AssemblyLine& line) {
    auto hasLabel = !line.label.empty();
    auto hasInstruction = !line.instruction.empty();
    auto hasOperands = !line.operands.empty();
    auto hasComment = !line.comment.empty();

    auto needsLabelPadding = hasInstruction || hasOperands || hasComment;
    auto needsInstructionPadding = hasOperands || hasComment;
    auto needsOperandsPadding = hasComment;

    if (hasLabel) {
        if (needsLabelPadding) {
            _out << std::format("{:<7s} ", line.label + ":");
        } else {
            _out << line.label + ":";
        }
    } else if (needsLabelPadding) {
        _out << std::format("{:<8s}", "");
    }

    if (hasInstruction) {
        if (needsInstructionPadding) {
            _out << std::format("{:<7s} ", line.instruction);
        } else {
            _out << line.instruction;
        }
    } else if (needsInstructionPadding) {
        _out << std::format("{:<8s}", "");
    }

    if (hasOperands) {
        if (needsOperandsPadding) {
            _out << std::format("{:<15s} ", line.operands);
        } else {
            _out << line.operands;
        }
    } else if (needsOperandsPadding) {
        _out << std::format("{:<16s}", "");
    }

    if (hasComment) {
        _out << std::format("; {}", line.comment);
    }

    _out << "\n";
}

void CodeGenerator::emitEmptyLine() {
    _out << "\n";
}

std::string CodeGenerator::output() const {
    return _out.str();
}
