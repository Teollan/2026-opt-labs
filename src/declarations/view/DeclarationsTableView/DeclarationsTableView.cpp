#include "DeclarationsTableView.hpp"

#include <format>
#include <variant>

#include "Declaration.hpp"

namespace {
std::string extractDeclarationIdentifier(const Declaration& decl) {
    return decl.identifier;
}

std::string extractDeclarationKind(const Declaration& decl) {
    switch (decl.kind) {
        case DeclarationKind::Program:
            return "Program";
        case DeclarationKind::Constant:
            return "Constant";
        default:
            return "?";
    }
}

std::string extractDeclarationType(const Declaration& decl) {
    if (!decl.type) {
        return "";
    }

    switch (decl.type.value()) {
        case Type::Integer:
            return "Integer";
        case Type::Float:
            return "Float";
        default:
            return "?";
    }
}

std::string extractDeclarationValue(const Declaration& decl) {
    if (!decl.value) {
        return "";
    }

    return std::visit(
        [](const auto& item) -> std::string {
            using T = std::decay_t<decltype(item)>;
            if constexpr (std::is_same_v<T, std::complex<int>>) {
                return std::format("{} + i*{}", item.real(), item.imag());
            } else if constexpr (std::is_same_v<T, std::complex<float>>) {
                return std::format(
                    "{:.3f} + i*{:.3f}", item.real(), item.imag()
                );
            } else {
                return std::format("{}", item);
            }
        },
        *decl.value
    );
}

std::string extractDeclarationModifiers(const Declaration& decl) {
    std::string result;

    for (size_t i = 0; i < decl.modifiers.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }

        switch (decl.modifiers[i]) {
            case TypeModifier::Complex:
                result += "Complex";
                break;
            default:
                result += "?";
                break;
        }
    }

    return result;
}
}  // namespace

DeclarationsTableView::DeclarationsTableView(
    std::string title,
    const DeclarationsTable& data
) :
    _table(std::move(title), data.entries()) {
    _table.addColumn("Identifier", "{:<20}", extractDeclarationIdentifier)
        .addColumn("Kind", "{:<10}", extractDeclarationKind)
        .addColumn("Type", "{:<10}", extractDeclarationType)
        .addColumn("Modifiers", "{:<15}", extractDeclarationModifiers)
        .addColumn("Value", "{:<30}", extractDeclarationValue);
}

void DeclarationsTableView::print() const {
    _table.print();
}
