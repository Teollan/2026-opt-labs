#pragma once

#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>

struct SemanticError : std::runtime_error {
    std::string message;
    size_t row;
    size_t column;

    SemanticError(std::string message, size_t row, size_t column);

    // Error message constants
    static constexpr auto DuplicateIdentifier(std::string identifier) {
        return std::format("Identifier '{}' is already declared", identifier);
    };
};
