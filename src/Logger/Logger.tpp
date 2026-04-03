template <typename T>
Logger<T>::Logger(
    std::string name,
    std::function<std::string(const T&)> formatter,
    std::ostream& out
) :
    _name(std::move(name)),
    _formatter(std::move(formatter)),
    _out(out) {}

template <typename T>
void Logger<T>::message(const T& data) {
    _messages.push_back(data);
    _out << _name << " | " << _formatter(data) << "\n";
}

template <typename T>
const std::vector<T>& Logger<T>::messages() const {
    return _messages;
}

template <typename T>
void Logger<T>::clear() {
    _messages.clear();
}
