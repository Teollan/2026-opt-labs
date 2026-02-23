#include <iostream>
#include <FileSource.hpp>
#include <SymbolStore.hpp>
#include <Tokenizer.hpp>
#include <TokensView.hpp>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: lab1 <filename>" << std::endl;

        return 1;
    }

    FileSource source(argv[1]);

    SymbolStore symbols;
    CharacterAttributes attributes;
    Tokenizer tokenizer(source, symbols, attributes);

    tokenizer.scan();

    TokensView view(std::cout);
    view.print(tokenizer.tokens(), symbols);

    for (const auto& error : tokenizer.errors()) {
        std::cout << std::format("Error [{}:{}]: {}", error.row + 1, error.column + 1, error.message) << std::endl;
    }

    return 0;
}
