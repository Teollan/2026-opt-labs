#include <Args.hpp>
#include <DeclarationsTableView.hpp>
#include <FileSource.hpp>
#include <IdentifiersTableView.hpp>
#include <LiteralsTableView.hpp>
#include <Logger.hpp>
#include <Parser.hpp>
#include <SemanticAnalyzer.hpp>
#include <SemanticError.hpp>
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
        .expectFlag("decl", "d")
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

    Logger<SemanticError> analyzerLogger("Semantics", errorFormatter);
    SemanticAnalyzer semntics(symbols, parser.tree(), analyzerLogger);
    semntics.analyze();

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

    if (args.getFlag("decl")) {
        DeclarationsTableView("\nDeclarations", semntics.declarations())
            .print();
    }

    return 0;
}
