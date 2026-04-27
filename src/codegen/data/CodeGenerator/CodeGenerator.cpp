#include "CodeGenerator.hpp"

#include <Declaration.hpp>
#include <algorithm>
#include <complex>
#include <format>
#include <variant>

CodeGenerator::CodeGenerator(const DeclarationsTable& declarations) :
    _declarations(declarations.entries()) {}

void CodeGenerator::generate() {
    _out.str("");
    _out.clear();

    emitDataSection();
    emitTextSection();
}

void CodeGenerator::emitDataSection() {
    const bool hasConstants =
        std::ranges::any_of(_declarations, [](const Declaration& declaration) {
            return declaration.kind == DeclarationKind::Constant;
        });

    if (!hasConstants) {
        return;
    }

    emitAssemblyLine({.instruction = "section", .operands = ".data"});
    emitEmptyLine();

    for (const auto& decl : _declarations) {
        if (decl.kind == DeclarationKind::Constant) {
            emitConstant(decl);
        }
    }

    emitEmptyLine();
}

void CodeGenerator::emitConstant(const Declaration& decl) {
    if (!decl.value.has_value()) {
        return;
    }

    std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::complex<int>>) {
                const auto& operands =
                    std::format("{}, {}", value.real(), value.imag());

                emitAssemblyLine({
                    .label = decl.identifier,
                    .instruction = "dd",
                    .operands = operands,
                });
            } else if constexpr (std::is_same_v<T, std::complex<float>>) {
                const auto& operands =
                    std::format("{:g}, {:g}", value.real(), value.imag());

                emitAssemblyLine({
                    .label = decl.identifier,
                    .instruction = "dd",
                    .operands = operands,
                });
            } else if constexpr (std::is_same_v<T, int>) {
                const auto& operands = std::format("{}", value);

                emitAssemblyLine({
                    .label = decl.identifier,
                    .instruction = "dd",
                    .operands = operands,
                });
            } else if constexpr (std::is_same_v<T, float>) {
                const auto& operands = std::format("{:g}", value);

                emitAssemblyLine({
                    .label = decl.identifier,
                    .instruction = "dd",
                    .operands = operands,
                });
            }
        },
        *decl.value
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
