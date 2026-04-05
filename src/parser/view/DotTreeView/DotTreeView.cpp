#include "DotTreeView.hpp"

#include <fstream>

DotTreeView::DotTreeView(const Tree<SyntaxData>& tree) : _tree(tree) {}

void DotTreeView::writeNode(
    std::ostream& out,
    const TreeNode<SyntaxData>& node,
    int& id
) const {
    int nodeId = id++;
    const auto& data = node.data();

    out << "    n" << nodeId
        << " [label=\"" << data.symbol << "\"]\n";

    for (const auto& child : node.children()) {
        int childId = id;
        writeNode(out, *child, id);

        int ruleId = id++;
        out << "    r" << ruleId
            << " [label=\"" << child->data().rule
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
