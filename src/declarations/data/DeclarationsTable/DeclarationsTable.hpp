#pragma once

#include <Declaration.hpp>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

class DeclarationsTable {
private:
    std::unordered_map<std::string, Declaration> _declarations;

public:
    [[nodiscard]] std::optional<Declaration> lookup(
        const std::string& identifier
    ) const;

    std::pair<const Declaration*, bool> declare(Declaration declaration);

    [[nodiscard]] std::vector<Declaration> entries() const;
};