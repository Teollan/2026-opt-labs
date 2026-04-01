#include <FileSource.hpp>
#include <Log.hpp>
#include <SymbolStore.hpp>
#include <Table.hpp>
#include <Tokenizer.hpp>
#include <iostream>

std::string getTypeLabel(SymbolType type) {
    switch (type) {
        case SymbolType::Ascii:
            return "Ascii";
        case SymbolType::MultiDelimiter:
            return "Multi-Delimiter";
        case SymbolType::Keyword:
            return "Keyword";
        case SymbolType::Literal:
            return "Literal";
        case SymbolType::Identifier:
            return "Identifier";
        default:
            return "N/A";
    }
}

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

    Table("\nTokens", tokenizer.tokens())
        .addColumn(
            "Code", "{:>5}",
            [](const auto& t) { return std::to_string(t.code); }
        )
        .addColumn(
            "Row", "{:>3}",
            [](const auto& t) { return std::to_string(t.row + 1); }
        )
        .addColumn(
            "Col", "{:>3}",
            [](const auto& t) { return std::to_string(t.column + 1); }
        )
        .addColumn(
            "Type", "{:<15}",
            [&](const auto& t) {
                return getTypeLabel(symbols.lookupType(t.code));
            }
        )
        .addColumn(
            "Value", "{:<30}",
            [&](const auto& t) { return std::string(symbols.lookup(t.code)); }
        )
        .print();

    Log("Tokenizer", tokenizer.errors())
        .setFormatter([](const auto& err) {
            return std::format(
                "Error [{}:{}]: {}", err.row + 1, err.column + 1, err.message
            );
        })
        .print();

    Table("\nIdentifiers", symbols.identifiers())
        .addColumn(
            "Code", "{:>5}",
            [](const auto& pair) { return std::to_string(pair.second); }
        )
        .addColumn(
            "Value", "{:<30}", [](const auto& pair) { return pair.first; }
        )
        .print();

    Table("\nLiterals", symbols.literals())
        .addColumn(
            "Code", "{:>5}",
            [](const auto& pair) { return std::to_string(pair.second); }
        )
        .addColumn(
            "Value", "{:<30}", [](const auto& pair) { return pair.first; }
        )
        .print();

    return 0;
}
