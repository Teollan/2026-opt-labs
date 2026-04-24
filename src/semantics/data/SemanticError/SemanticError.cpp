#include "SemanticError.hpp"

#include <utility>

SemanticError::SemanticError(std::string message, size_t row, size_t column) :
    std::runtime_error(message),
    message(std::move(message)),
    row(row),
    column(column) {}
