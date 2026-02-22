#include <StringSource.class.hpp>
#include <SymbolStore.class.hpp>
#include <Tokenizer.class.hpp>
#include <format>
#include <iostream>
#include <string>

std::string getTokenGroupLabel(const SymbolStore& symbols, unsigned int code) {
    auto type = symbols.lookupType(code);

    switch (type) {
        case SymbolType::Character:
            return "Character";
        case SymbolType::Keyword:
            return "Keyword";
        case SymbolType::Literal:
            return "Literal";
        case SymbolType::Identifier:
            return "Identifier";
    }
}

int main() {
    StringSource source(
        "PROGRAM TEST;\n"
        "CONST\n"
        "  (* This is a comment. *)\n"
        "  X = '10$EXP(20)'#;\n"
        "  Y = '30,40';\n"
        "BEGIN\n"
        "END.");

    SymbolStore symbols;
    CharacterAttributes attributes;
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    std::cout << std::format("{:>5}", "Code") << " | " << std::format("{:>3}", "Row") << " | "
              << std::format("{:>3}", "Col") << " | " << std::format("{:10}", "Group") << " | "
              << "Value" << std::endl;

    for (const auto& token : tokenizer.tokens()) {
        std::cout << std::format("{:>5}", token.code) << " | " << std::format("{:>3}", token.row + 1) << " | "
                  << std::format("{:>3}", token.column + 1) << " | " << std::format("{:10}", getTokenGroupLabel(symbols, token.code))
                  << " | " << symbols.lookup(token.code) << std::endl;
    }

    for (const auto& error : tokenizer.errors()) {
        std::cout << "Error: " << error.message << std::endl;
    }

    return 0;
}
