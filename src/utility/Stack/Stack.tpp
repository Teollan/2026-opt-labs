template <typename T>
Stack<T>::Stack() : items() {}

template <typename T>
Stack<T>::Stack(const std::vector<T>& items) : items(items) {}

template <typename T>
void Stack<T>::push(const T& item) {
    items.push_back(item);
}

template <typename T>
std::optional<T> Stack<T>::pop() {
    if (items.empty()) {
        return std::nullopt;
    }

    T item = std::move(items.back());
    items.pop_back();

    return item;
}

template <typename T>
std::optional<T> Stack<T>::peek() const {
    if (items.empty()) {
        return std::nullopt;
    }

    return items.back();
}

template <typename T>
bool Stack<T>::isEmpty() const {
    return items.empty();
}

template <typename T>
size_t Stack<T>::size() const {
    return items.size();
}