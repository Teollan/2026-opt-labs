template <typename T>
TreeView<T>::TreeView(std::string title, const Tree<T>& tree, std::ostream& out) :
    _title(std::move(title)),
    _tree(tree),
    _out(out),
    _nodeFormatter(nullptr),
    _edgeFormatter(nullptr) {}

template <typename T>
TreeView<T>& TreeView<T>::setNodeFormatter(
    std::function<std::string(const T&)> formatter
) {
    _nodeFormatter = std::move(formatter);

    return *this;
}

template <typename T>
TreeView<T>& TreeView<T>::setEdgeFormatter(
    std::function<std::string(const T&)> formatter
) {
    _edgeFormatter = std::move(formatter);

    return *this;
}

template <typename T>
void TreeView<T>::print() const {
    auto& root = _tree.root();

    _out << _title << "\n";
    _out << _nodeFormatter(root.data()) << "\n";

    for (const auto& child : root.children()) {
        printNode(*child, "", child == root.children().back());
    }
}

template <typename T>
void TreeView<T>::printNode(
    const TreeNode<T>& node,
    const std::string& prefix,
    bool isLast
) const {
    std::string source = isLast
        ? Box::UpAndRight
        : Box::VerticalAndRight;

    std::string horizontalConnector = Box::Horizontal;

    std::string edgeLabel = _edgeFormatter(node.data());

    std::string nodeLabel = _nodeFormatter(node.data());

    std::string verticalConnector = isLast
        ? " "
        : Box::Vertical;

    _out << prefix << source << horizontalConnector;

    _out << edgeLabel << horizontalConnector;

    _out << nodeLabel << "\n";

    std::string childPrefix = prefix + verticalConnector + std::string(edgeLabel.size() + 2, ' ');

    for (const auto& child : node.children()) {
        printNode(*child, childPrefix, child == node.children().back());
    }
}
