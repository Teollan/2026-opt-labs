#pragma once

#include <Type.hpp>
#include <complex>
#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include <vector>

using Value = std::variant<int, float, std::complex<int>, std::complex<float>>;

enum class DeclarationKind {
    Constant,
    Program,
};

struct Declaration {
    std::string identifier;
    DeclarationKind kind;
    std::optional<Type> type;
    std::optional<Value> value;
    std::vector<TypeModifier> modifiers;
    size_t row = 0;
    size_t column = 0;
};