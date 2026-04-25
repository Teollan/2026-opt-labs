#pragma once

#include <DeclarationsTable.hpp>
#include <sstream>
#include <string>

struct AssemblyLine {
    std::string label = "";
    std::string instruction = "";
    std::string operands = "";
    std::string comment = "";
};

class CodeGenerator {
private:
    const DeclarationsTable& _declarations;
    std::ostringstream _out;

    void emitDataSection();
    void emitTextSection();
    void emitConstant(const Declaration& decl);
    void emitAssemblyLine(const AssemblyLine& line);
    void emitEmptyLine();

    std::string findProgramName();

public:
    explicit CodeGenerator(const DeclarationsTable& declarations);

    void generate();
    [[nodiscard]] std::string output() const;
};
