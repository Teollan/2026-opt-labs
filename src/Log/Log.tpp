template <typename T>
Log<T>::Log(std::string name, std::vector<T> data, std::ostream& out) :
    name(std::move(name)), data(std::move(data)), out(out) {}

template <typename T>
Log<T>& Log<T>::setFormatter(std::function<std::string(const T&)> formatter) {
    this->formatter = std::move(formatter);

    return *this;
}

template <typename T>
void Log<T>::print() const {
    for (const auto& item : data) {
        out << name << " | " << formatter(item) << "\n";
    }
}
