#include <Args.hpp>
#include <CodeGenerator.hpp>
#include <DeclarationsTableView.hpp>
#include <FileSource.hpp>
#include <IdentifiersTableView.hpp>
#include <LexicalError.hpp>
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
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    Args args(argc, argv);
    args.expectString("source", "s", true)
        .expectString("output", "o", false)
        .expectFlag("tokens", "t")
        .expectFlag("tree", "T")
        .expectFlag("identifiers", "i")
        .expectFlag("literals", "l")
        .expectFlag("decl", "d")
        .expectFlag("asm", "a")
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

    Logger<SemanticError> analyzerLogger("Semantics", errorFormatter);
    SemanticAnalyzer semantics(symbols, parser.tree(), analyzerLogger);
    semantics.analyze();

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
        DeclarationsTableView("\nDeclarations", semantics.declarations())
            .print();
    }

    const bool hasErrors = !tokenizerLogger.messages().empty() ||
                           !parserLogger.messages().empty() ||
                           !analyzerLogger.messages().empty();

    if (!hasErrors) {
        CodeGenerator codegen(semantics.declarations());
        codegen.generate();

        const auto outputPath = args.getString("output");
        if (!outputPath.empty()) {
            std::ofstream file(outputPath);
            file << codegen.output();
            std::cout << "Assembly written to: " << outputPath << "\n";
        }

        if (args.getFlag("asm")) {
            std::cout << "\n" << codegen.output();
        }
    }

    return 0;
}
