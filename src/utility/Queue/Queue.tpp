template <typename T>
Queue<T>::Queue() : _items() {}

template <typename T>
Queue<T>::Queue(const std::vector<T>& items) :
    _items(items.begin(), items.end()) {}

template <typename T>
void Queue<T>::push(const T& item) {
    _items.push_back(item);
}

template <typename T>
std::optional<T> Queue<T>::pop() {
    if (_items.empty()) {
        return std::nullopt;
    }

    T item = std::move(_items.front());
    _items.pop_front();

    return item;
}

template <typename T>
std::optional<T> Queue<T>::popUntil(
    const std::function<bool(const T&)>& predicate
) {
    while (!_items.empty()) {
        if (predicate(_items.front())) {
            return _items.front();
        }

        _items.pop_front();
    }

    return std::nullopt;
}

template <typename T>
void Queue<T>::clear() {
    _items.clear();
}

template <typename T>
std::optional<T> Queue<T>::peek() const {
    if (_items.empty()) {
        return std::nullopt;
    }

    return _items.front();
}

template <typename T>
bool Queue<T>::isEmpty() const {
    return _items.empty();
}

template <typename T>
size_t Queue<T>::size() const {
    return _items.size();
}
