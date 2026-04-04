#include <windows.h>

#include <Args.hpp>
#include <FileSource.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <SymbolStore.hpp>
#include <SyntaxTreeView.hpp>
#include <TokensTableView.hpp>
#include <Tokenizer.hpp>
#include <format>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    Args args(argc, argv);
    args.expectString("source", "s", true)
        .expectFlag("tokens", "t")
        .expectFlag("tree", "T")
        .parse();

    FileSource source(args.getString("source"));
    SymbolStore symbols;
    CharacterAttributes attributes;
    Logger<Error> logger("Compiler", [](const auto& err) {
        return std::format(
            "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
        );
    });
    Tokenizer tokenizer(source, symbols, attributes, logger);
    tokenizer.scan();

    Parser parser(symbols, tokenizer.tokens(), logger);
    parser.parse();

    if (args.getFlag("tokens")) {
        TokensTableView("\nTokens", symbols, tokenizer.tokens()).print();
    }

    if (args.getFlag("tree")) {
        SyntaxTreeView("\nSyntax Tree", parser.tree()).print();
    }

    std::cout << "\nlab2.exe executed successfully.\n";

    return 0;
}
