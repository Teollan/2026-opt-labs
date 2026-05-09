#pragma once

#include <cstddef>
#include <format>
#include <string>

struct SemanticError {
    std::string message;
    size_t row;
    size_t column;

    // Error message constants
    static auto DuplicateIdentifier(
        std::string identifier,
        size_t originalRow,
        size_t originalColumn
    ) {
        return std::format(
            "Identifier '{}' is already declared at [{}:{}]", identifier,
            originalRow + 1, originalColumn + 1
        );
    };
};
