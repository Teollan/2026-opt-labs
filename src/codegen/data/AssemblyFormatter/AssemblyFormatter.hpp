#pragma once

#include <string>

struct AssemblyFormatterConfig {
    unsigned int labelWidth = 0;
    unsigned int instructionWidth = 0;
    unsigned int operandsWidth = 0;
};

class AssemblyFormatter {
private:
    AssemblyFormatterConfig _config;

    static std::string formatLabel(const std::string& label);
    static std::string formatComment(const std::string& comment);

    static std::string indent(unsigned int width);
    static std::string pad(const std::string& text, unsigned int width);

public:
    explicit AssemblyFormatter(AssemblyFormatterConfig config);

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