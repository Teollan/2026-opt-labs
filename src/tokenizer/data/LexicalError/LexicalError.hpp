#pragma once

#include <cstddef>
#include <format>
#include <string>

struct LexicalError {
    std::string message;
    size_t row;
    size_t column;

    static auto InvalidMultiDelimiter(const std::string& token) {
        return std::format("'{}' is not a valid multi-delimiter", token);
    }

    static auto UnclosedComment() {
        return std::string("Comment not closed");
    }

    static auto InvalidCharacter(char character) {
        return std::format("Invalid character '{}'", character);
    }
};
