#include "DeclarationsTable.hpp"

#include <algorithm>
#include <ranges>

std::optional<Declaration> DeclarationsTable::lookup(
    const std::string& identifier
) const {
    if (!_declarations.contains(identifier)) {
        return std::nullopt;
    }

    return _declarations.at(identifier);
}

std::pair<const Declaration*, bool> DeclarationsTable::declare(
    Declaration declaration
) {
    auto [iterator, inserted] = _declarations.try_emplace(
        declaration.identifier, std::move(declaration)
    );

    return {&iterator->second, inserted};
}

std::vector<Declaration> DeclarationsTable::entries() const {
    auto result = std::ranges::to<std::vector<Declaration>>(
        std::views::values(_declarations)
    );

    std::ranges::sort(result, {}, &Declaration::identifier);

    return result;
}
