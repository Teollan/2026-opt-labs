#include <windows.h>

#include <FileSource.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <SymbolStore.hpp>
#include <SyntaxTreeView.hpp>
#include <Tokenizer.hpp>
#include <format>
#include <iostream>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    if (argc < 2) {
        std::cerr << "Usage: lab2 <filename>" << std::endl;
        return 1;
    }

    FileSource source(argv[1]);
    SymbolStore symbols;
    CharacterAttributes attributes;
    Logger<Error> logger("Parser", [](const auto& err) {
        return std::format(
            "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
        );
    });
    Tokenizer tokenizer(source, symbols, attributes, logger);
    tokenizer.scan();

    Parser parser(symbols, tokenizer.tokens(), logger);
    parser.parse();

    SyntaxTreeView("\nSyntax Tree", parser.tree()).print();

    return 0;
}
