#pragma once

#include <string>

class AssemblyFormatter {
private:
    unsigned int _labelWidth;
    unsigned int _instructionWidth;
    unsigned int _operandsWidth;

    static std::string formatLabel(const std::string& label);
    static std::string formatComment(const std::string& comment);

    static std::string indent(unsigned int width);
    static std::string pad(const std::string& text, unsigned int width);

public:
    AssemblyFormatter(
        unsigned int labelWidth,
        unsigned int instructionWidth,
        unsigned int operandsWidth
    );

    [[nodiscard]] std::string sectionLine(const std::string& name) const;
    [[nodiscard]] std::string instructionLine(
        const std::string& instruction
    ) const;
    [[nodiscard]] std::string instructionLine(
        const std::string& instruction,
        const std::string& operands
    ) const;
    [[nodiscard]] std::string constantLine(
        const std::string& label,
        const std::string& size,
        const std::string& value
    ) const;

    [[nodiscard]] std::string commentLine(const std::string& comment) const;
    [[nodiscard]] std::string labelLine(const std::string& label) const;
    [[nodiscard]] std::string blankLine() const;
};