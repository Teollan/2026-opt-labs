#pragma once

#include <vector>
#include <optional>
#include <functional>

template <typename T>
class Stack {
private:
  std::vector<T> _items;

public:
    Stack();
    Stack(const std::vector<T>& items);

    void push(const T& item);

    std::optional<T> pop();
    std::optional<T> popUntil(const std::function<bool(const T&)>& predicate);
    
    void clear();

    std::optional<T> peek() const;
    bool isEmpty() const;
    size_t size() const;
};

#include "Stack.tpp"