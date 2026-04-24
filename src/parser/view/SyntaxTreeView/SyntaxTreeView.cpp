#include "SyntaxTreeView.hpp"

#include <format>

#include <Rules.hpp>

SyntaxTreeView::SyntaxTreeView(
    std::string title,
    const Tree<SyntaxData>& tree,
    const SymbolStore& symbols
) :
    _tree(std::move(title), tree) {
    _tree
        .setNodeFormatter([&symbols](const auto& data) {
            if (data.rule == RuleKey::Terminal) {
                return symbols.lookup(data.token->code);
            }
            return RULES.at(data.rule).output;
        })
        .setEdgeFormatter([](const auto& data) {
            if (data.rule == RuleKey::Terminal) {
                return std::string("(T)");
            }
            return std::format("({})", RULES.at(data.rule).id);
        });
}

void SyntaxTreeView::print() const {
    _tree.print();
}
