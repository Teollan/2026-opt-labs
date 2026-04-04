#include <windows.h>

#include <FileSource.hpp>
#include <IdentifiersTableView.hpp>
#include <LiteralsTableView.hpp>
#include <Logger.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>
#include <TokensTableView.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    if (argc < 2) {
        std::cerr << "Usage: lab1 <filename>" << std::endl;
        return 1;
    }

    FileSource source(argv[1]);
    SymbolStore symbols;
    CharacterAttributes attributes;
    Logger<Error> logger("Tokenizer", [](const auto& err) {
        return std::format(
            "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
        );
    });
    Tokenizer tokenizer(source, symbols, attributes, logger);
    tokenizer.scan();

    TokensTableView("\nTokens", symbols, tokenizer.tokens()).print();

    IdentifiersTableView("\nIdentifiers", symbols.identifiers()).print();

    LiteralsTableView("\nLiterals", symbols.literals()).print();

    return 0;
}
