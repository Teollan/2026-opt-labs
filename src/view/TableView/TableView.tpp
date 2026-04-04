template <typename T>
TableView<T>::TableView(
    std::string title,
    std::vector<T> data,
    std::ostream& out
) :
    _title(std::move(title)),
    _data(std::move(data)),
    _out(out) {}

template <typename T>
TableView<T>& TableView<T>::addColumn(
    const std::string& colName,
    const std::string& format,
    std::function<std::string(const T&)> extractor
) {
    _columns.push_back({colName, format, std::move(extractor)});

    return *this;
}

template <typename T>
void TableView<T>::print() const {
    if (!_title.empty()) {
        _out << _title << "\n";
    }

    _out << border(Box::DownAndRight, Box::DownAndHorizontal, Box::DownAndLeft) << "\n";
    _out << header() << "\n";
    _out << border(Box::VerticalAndRight, Box::VerticalAndHorizontal, Box::VerticalAndLeft) << "\n";

    for (const auto& item : _data) {
        _out << row(item) << "\n";
    }

    _out << border(Box::UpAndRight, Box::UpAndHorizontal, Box::UpAndLeft) << "\n";
}

template <typename T>
std::string TableView<T>::border(
    const std::string& left,
    const std::string& junction,
    const std::string& right
) const {
    std::string result = left;

    for (size_t i = 0; i < _columns.size(); ++i) {
        size_t width = columnWidth(_columns[i]);

        for (size_t j = 0; j < width + 2; ++j) {
            result += Box::Horizontal;
        }

        result += (i < _columns.size() - 1) ? junction : right;
    }

    return result;
}

template <typename T>
std::string TableView<T>::header() const {
    std::string result;
    result += Box::Vertical;

    for (const auto& column : _columns) {
        result += std::format(" {:<{}} ", column.name, columnWidth(column));
        result += Box::Vertical;
    }

    return result;
}

template <typename T>
std::string TableView<T>::row(const T& item) const {
    std::string result;
    result += Box::Vertical;

    for (const auto& column : _columns) {
        std::string value = column.extractor(item);
        result += " " +
                  std::vformat(column.format, std::make_format_args(value)) +
                  " ";
        result += Box::Vertical;
    }

    return result;
}

template <typename T>
size_t TableView<T>::columnWidth(const Column& column) const {
    return std::max(parseWidth(column.format), column.name.length());
}

template <typename T>
size_t TableView<T>::parseWidth(const std::string& format) {
    size_t width = 0;

    for (char character : format) {
        if (character >= '0' && character <= '9') {
            width = (width * 10) + (character - '0');
        }
    }

    return width;
}
