template <typename T>
Stack<T>::Stack() : _items() {}

template <typename T>
Stack<T>::Stack(const std::vector<T>& items) : _items(items) {}

template <typename T>
void Stack<T>::push(const T& item) {
    _items.push_back(item);
}

template <typename T>
std::optional<T> Stack<T>::pop() {
    if (_items.empty()) {
        return std::nullopt;
    }

    T item = std::move(_items.back());
    _items.pop_back();

    return item;
}

template <typename T>
std::optional<T> Stack<T>::popUntil(const std::function<bool(const T&)>& predicate) {
    while (!_items.empty()) {
        if (predicate(_items.back())) {
            return _items.back();
        }

        _items.pop_back();
    }

    return std::nullopt;
}

template <typename T>
void Stack<T>::clear() {
    _items.clear();
}

template <typename T>
std::optional<T> Stack<T>::peek() const {
    if (_items.empty()) {
        return std::nullopt;
    }

    return _items.back();
}

template <typename T>
bool Stack<T>::isEmpty() const {
    return _items.empty();
}

template <typename T>
size_t Stack<T>::size() const {
    return _items.size();
}