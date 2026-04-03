template <typename T>
Tree<T>::Tree(T data) : _root(std::make_unique<TreeNode<T>>(data)) {}

template <typename T>
TreeNode<T>& Tree<T>::root() {
    return *_root;
}

template <typename T>
const TreeNode<T>& Tree<T>::root() const {
    return *_root;
}
