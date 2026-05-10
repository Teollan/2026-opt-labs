#include "AssemblyFormatter.hpp"

#include <format>

AssemblyFormatter::AssemblyFormatter(AssemblyFormatterConfig config) :
    _config(config) {}

std::string AssemblyFormatter::commentLine(const std::string& comment) const {
    return formatComment(comment) + "\n";
}

std::string AssemblyFormatter::blankLine() const {
    return "\n";
}

std::string AssemblyFormatter::sectionLine(const std::string& name) const {
    return indent(_config.labelWidth) + std::format("section {}:\n", name);
}

std::string AssemblyFormatter::labelLine(const std::string& label) const {
    return formatLabel(label) + "\n";
}

std::string AssemblyFormatter::instructionLine(
    const std::string& instruction
) const {
    return indent(_config.labelWidth) + instruction + "\n";
}

std::string AssemblyFormatter::instructionLine(
    const std::string& instruction,
    const std::string& operands
) const {
    auto instructionPad = _config.instructionWidth > 0 ? _config.instructionWidth - 1 : 0;

    std::string line = indent(_config.labelWidth);
    line += pad(instruction, instructionPad);
    line += " ";
    line += operands;
    line += "\n";

    return line;
}

std::string AssemblyFormatter::constantLine(
    const std::string& label,
    const std::string& size,
    const std::string& value
) const {
    auto labelPad = _config.labelWidth > 0 ? _config.labelWidth - 1 : 0;
    auto instructionPad = _config.instructionWidth > 0 ? _config.instructionWidth - 1 : 0;

    std::string line = pad(formatLabel(label), labelPad);
    line += " ";
    line += pad(size, instructionPad);
    line += " ";
    line += value;
    line += "\n";

    return line;
}

std::string AssemblyFormatter::formatLabel(const std::string& label) {
    return std::format("{}:", label);
}

std::string AssemblyFormatter::formatComment(const std::string& comment) {
    return std::format("; {}", comment);
}

std::string AssemblyFormatter::indent(unsigned int width) {
    return std::string(width, ' ');
}

std::string AssemblyFormatter::pad(
    const std::string& text,
    unsigned int width
) {
    if (text.length() >= width) {
        return text;
    }

    return text + std::string(width - text.length(), ' ');
}