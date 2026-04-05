#include <windows.h>

#include <Args.hpp>
#include <FileSource.hpp>
#include <IdentifiersTableView.hpp>
#include <LiteralsTableView.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <SymbolStore.hpp>
#include <SyntaxError.hpp>
#include <SyntaxTreeView.hpp>
#include <Tokenizer.hpp>
#include <TokensTableView.hpp>
#include <format>
#include <iostream>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    Args args(argc, argv);
    args.expectString("source", "s", true)
        .expectFlag("tokens", "t")
        .expectFlag("tree", "T")
        .expectFlag("identifiers", "i")
        .expectFlag("literals", "l")
        .parse();

    auto errorFormatter = [](const auto& err) {
        return std::format(
            "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
        );
    };

    SymbolStore symbols;
    CharacterAttributes attributes;

    FileSource source(args.getString("source"));
    Logger<Error> tokenizerLogger("Tokenizer", errorFormatter);
    Tokenizer tokenizer(source, symbols, attributes, tokenizerLogger);
    tokenizer.scan();

    Logger<SyntaxError> parserLogger("Parser", errorFormatter);
    Parser parser(symbols, tokenizer.tokens(), parserLogger);
    parser.parse();

    if (args.getFlag("tokens")) {
        TokensTableView("\nTokens", symbols, tokenizer.tokens()).print();
    }

    if (args.getFlag("identifiers")) {
        IdentifiersTableView("\nIdentifiers", symbols.identifiers()).print();
    }

    if (args.getFlag("literals")) {
        LiteralsTableView("\nLiterals", symbols.literals()).print();
    }

    if (args.getFlag("tree")) {
        SyntaxTreeView("\nSyntax Tree", parser.tree()).print();
    }

    return 0;
}
