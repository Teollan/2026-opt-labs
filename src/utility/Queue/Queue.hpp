#pragma once

#include <deque>
#include <functional>
#include <optional>
#include <vector>

template <typename T>
class Queue {
private:
    std::deque<T> _items;

public:
    Queue();
    Queue(const std::vector<T>& items);

    void push(const T& item);

    std::optional<T> pop();
    std::optional<T> popUntil(const std::function<bool(const T&)>& predicate);

    void clear();

    std::optional<T> peek() const;
    bool isEmpty() const;
    size_t size() const;
};

#include "Queue.tpp"
