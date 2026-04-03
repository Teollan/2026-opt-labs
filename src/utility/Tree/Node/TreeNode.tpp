template <typename T>
TreeNode<T>::TreeNode(T data) : _data(data) {}

template <typename T>
const T& TreeNode<T>::data() const {
    return _data;
}

template <typename T>
const std::vector<std::unique_ptr<TreeNode<T>>>& TreeNode<T>::children() const {
    return _children;
}

template <typename T>
TreeNode<T>& TreeNode<T>::grow(T data) {
    _children.push_back(std::make_unique<TreeNode<T>>(data));
    return *_children.back();
}
