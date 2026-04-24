#include "DotTreeView.hpp"

#include <fstream>

#include <Rules.hpp>

DotTreeView::DotTreeView(
    const Tree<SyntaxData>& tree,
    const SymbolStore& symbols
) :
    _tree(tree), _symbols(symbols) {}

std::string DotTreeView::nodeLabel(const SyntaxData& data) const {
    if (data.rule == RuleKey::Terminal) {
        return _symbols.lookup(data.token->code);
    }
    return RULES.at(data.rule).output;
}

std::string DotTreeView::edgeLabel(const SyntaxData& data) const {
    if (data.rule == RuleKey::Terminal) {
        return "T";
    }
    return RULES.at(data.rule).id;
}

void DotTreeView::writeNode(
    std::ostream& out,
    const TreeNode<SyntaxData>& node,
    int& id
) const {
    int nodeId = id++;
    const auto& data = node.data();

    out << "    n" << nodeId << " [label=\"" << nodeLabel(data) << "\"]\n";

    for (const auto& child : node.children()) {
        int childId = id;
        writeNode(out, *child, id);

        int ruleId = id++;
        out << "    r" << ruleId << " [label=\"" << edgeLabel(child->data())
            << "\" shape=circle width=0.3 fixedsize=true"
            << " fontsize=10]\n";

        out << "    n" << nodeId << " -> r" << ruleId
            << " [arrowhead=none]\n";
        out << "    r" << ruleId << " -> n" << childId << "\n";
    }
}

void DotTreeView::write(const std::string& filename) const {
    std::ofstream out(filename);

    out << "digraph SyntaxTree {\n"
        << "    rankdir=TB\n"
        << "    node [shape=none fontname=\"Consolas\" fontsize=11]\n"
        << "    edge [arrowsize=0.6]\n\n";

    int id = 0;
    writeNode(out, _tree.root(), id);

    out << "}\n";
}
