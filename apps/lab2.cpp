#include <FileSource.hpp>
#include <Log.hpp>
#include <Parser.hpp>
#include <SymbolStore.hpp>
#include <Table.hpp>
#include <Tokenizer.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: lab2 <filename>" << std::endl;
        return 1;
    }

    FileSource source(argv[1]);
    SymbolStore symbols;
    CharacterAttributes attributes;
    Tokenizer tokenizer(source, symbols, attributes);
    tokenizer.scan();

    Log("Tokenizer", tokenizer.errors())
        .setFormatter([](const auto& err) {
            return std::format(
                "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
            );
        })
        .print();

    Parser parser(symbols, tokenizer.tokens());
    parser.parse();

    return 0;
}
