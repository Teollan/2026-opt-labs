#include <windows.h>

#include <Args.hpp>
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

    Args args(argc, argv);
    args.expectString("source", "s", true)
        .expectFlag("tokens", "t")
        .expectFlag("identifiers", "i")
        .expectFlag("literals", "l")
        .parse();

    SymbolStore symbols;
    CharacterAttributes attributes;

    FileSource source(args.getString("source"));
    Logger<Error> logger("Tokenizer", [](const auto& err) {
        return std::format(
            "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
        );
    });
    Tokenizer tokenizer(source, symbols, attributes, logger);
    tokenizer.scan();

    if (args.getFlag("tokens")) {
        TokensTableView("\nTokens", symbols, tokenizer.tokens()).print();
    }

    if (args.getFlag("identifiers")) {
        IdentifiersTableView("\nIdentifiers", symbols.identifiers()).print();
    }

    if (args.getFlag("literals")) {
        LiteralsTableView("\nLiterals", symbols.literals()).print();
    }

    std::cout << "\nlab1.exe executed successfully.\n";

    return 0;
}
