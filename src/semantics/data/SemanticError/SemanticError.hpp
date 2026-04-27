#pragma once

#include <cstddef>
#include <format>
#include <string>

struct SemanticError {
    std::string message;
    size_t row;
    size_t column;

    // Error message constants
    static auto DuplicateIdentifier(std::string identifier) {
        return std::format("Identifier '{}' is already declared", identifier);
    };
};
