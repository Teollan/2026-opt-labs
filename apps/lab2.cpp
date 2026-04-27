#include <Args.hpp>
#include <DotTreeView.hpp>
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

int main(int argc, char* argv[]) {
    Args args(argc, argv);
    args.expectString("source", "s", true)
        .expectFlag("tokens", "t")
        .expectFlag("tree", "T")
        .expectFlag("identifiers", "i")
        .expectFlag("literals", "l")
        .expectString("dot", "d", false)
        .parse();

    auto errorFormatter = [](const auto& err) {
        return std::format(
            "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
        );
    };

    SymbolStore symbols;
    CharacterAttributes attributes;

    FileSource source(args.getString("source"));
    Logger<LexicalError> tokenizerLogger("Tokenizer", errorFormatter);
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
        SyntaxTreeView("\nSyntax Tree", parser.tree(), symbols).print();
    }

    if (!args.getString("dot").empty()) {
        DotTreeView(parser.tree(), symbols).write(args.getString("dot"));
    }

    return 0;
}
