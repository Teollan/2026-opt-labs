template <typename T>
Table<T>::Table(std::string name, std::vector<T> data, std::ostream& out) :
    name(std::move(name)), data(std::move(data)), out(out) {}

template <typename T>
Table<T>& Table<T>::addColumn(
    const std::string& colName,
    const std::string& format,
    std::function<std::string(const T&)> extractor
) {
    columns.push_back({colName, format, std::move(extractor)});

    return *this;
}

template <typename T>
void Table<T>::print() const {
    if (!name.empty()) {
        out << name << "\n";
    }

    std::string borderLine = border();

    out << borderLine << "\n";
    out << header() << "\n";
    out << borderLine << "\n";

    for (const auto& item : data) {
        out << row(item) << "\n";
    }

    out << borderLine << "\n";
}

template <typename T>
std::string Table<T>::border() const {
    std::string result = "+";

    for (const auto& col : columns) {
        size_t width = columnWidth(col);
        result += std::string(width + 2, '-') + "+";
    }

    return result;
}

template <typename T>
std::string Table<T>::header() const {
    std::string result = "|";

    for (const auto& column : columns) {
        result += std::format(" {:<{}} |", column.name, columnWidth(column));
    }

    return result;
}

template <typename T>
std::string Table<T>::row(const T& item) const {
    std::string result = "|";

    for (const auto& column : columns) {
        std::string value = column.extractor(item);
        result += " " + std::vformat(column.format, std::make_format_args(value)) + " |";
    }

    return result;
}

template <typename T>
size_t Table<T>::columnWidth(const Column& column) const {
    return std::max(parseWidth(column.format), column.name.length());
}

template <typename T>
size_t Table<T>::parseWidth(const std::string& fmt) {
    size_t width = 0;

    for (char chr : fmt) {
        if (chr >= '0' && chr <= '9') {
            width = (width * 10) + (chr - '0');
        }
    }

    return width;
}
