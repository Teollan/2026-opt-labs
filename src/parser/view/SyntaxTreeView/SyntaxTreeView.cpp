#include "SyntaxTreeView.hpp"

#include <format>

SyntaxTreeView::SyntaxTreeView(
    std::string title,
    const Tree<SyntaxData>& tree
) :
    _tree(std::move(title), tree) {
    _tree
        .setNodeFormatter([](const auto& data) { return data.symbol; })
        .setEdgeFormatter([](const auto& data) {
            return std::format("({})", data.rule);
        });
}

void SyntaxTreeView::print() const {
    _tree.print();
}
