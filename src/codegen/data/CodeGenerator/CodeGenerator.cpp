#include "CodeGenerator.hpp"

#include <Declaration.hpp>
#include <algorithm>
#include <complex>
#include <format>
#include <variant>

CodeGenerator::CodeGenerator(const DeclarationsTable& declarations) :
    _declarations(declarations) {}

void CodeGenerator::generate() {
    _out.str("");
    _out.clear();

    emitDataSection();
    emitTextSection();
}

void CodeGenerator::emitDataSection() {
    const auto entries = _declarations.entries();
    const bool hasConstants =
        std::ranges::any_of(entries, [](const Declaration& declaration) {
            return declaration.kind == DeclarationKind::Constant;
        });

    if (!hasConstants) {
        return;
    }

    emitAssemblyLine({.instruction = "section", .operands = ".data"});
    emitEmptyLine();

    for (const auto& decl : entries) {
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
    const auto program = findProgramName();

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
        _out << std::format("{:<8s} ", "");
    }

    if (hasOperands) {
        if (needsOperandsPadding) {
            _out << std::format("{:<15s} ", line.operands);
        } else {
            _out << line.operands;
        }
    } else if (needsOperandsPadding) {
        _out << std::format("{:<16s} ", "");
    }

    if (hasComment) {
        _out << std::format("; {}", line.comment);
    }

    _out << "\n";
}

void CodeGenerator::emitEmptyLine() {
    _out << "\n";
}

std::string CodeGenerator::findProgramName() {
    const auto entries = _declarations.entries();
    const auto programNameIterator =
        std::ranges::find_if(entries, [](const Declaration& decl) {
            return decl.kind == DeclarationKind::Program;
        });

    if (programNameIterator == entries.end()) {
        return "main";
    }

    return programNameIterator->identifier;
}

std::string CodeGenerator::output() const {
    return _out.str();
}
