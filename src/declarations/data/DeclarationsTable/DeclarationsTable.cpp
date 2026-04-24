#include "DeclarationsTable.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>

std::optional<Declaration> DeclarationsTable::lookup(
    const std::string& identifier
) const {
    if (!_declarations.contains(identifier)) {
        return std::nullopt;
    }

    return _declarations.at(identifier);
}

void DeclarationsTable::declare(Declaration declaration) {
    if (lookup(declaration.identifier)) {
        throw std::runtime_error("");
    }

    _declarations[declaration.identifier] = declaration;
}

std::vector<Declaration> DeclarationsTable::entries() const {
    auto result = std::ranges::to<std::vector<Declaration>>(
        std::views::values(_declarations)
    );

    std::ranges::sort(result, {}, &Declaration::identifier);

    return result;
}
