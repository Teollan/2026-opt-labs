#pragma once

#include <Type.hpp>
#include <complex>
#include <cstddef>
#include <optional>
#include <string>
#include <variant>

using Value = std::variant<std::complex<int>, std::complex<float>>;

enum class DeclarationKind {
    Constant,
    Program,
};

struct Declaration {
    std::string identifier;
    DeclarationKind kind;
    std::optional<Type> type;
    std::optional<Value> value;
    size_t row = 0;
    size_t column = 0;
};